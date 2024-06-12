#include "chartview.h"
#include <QLayout>
#include <QDebug>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QDebug>
#include <QtCharts/QScatterSeries>

ChartViewSettings::ChartViewSettings(QObject *parent) : SettingsAbstract(parent)
{
//    mapSettingsDefauold["gColor.varColor"] = QColor(0x5f, 0x5f, 0x5f);
//    mapSettingsDefauold["gColor.gridColor"] = QColor(0x9a, 0x9a, 0x9a);
//    mapSettingsDefauold["gColor.subGridColor"] = QColor(0xbe, 0xbe, 0xbe);
//    mapSettingsDefauold["gColor.bgColor1"] = QColor(0xf9, 0xf9, 0xf9);
//    mapSettingsDefauold["gColor.bgColor2"] = QColor(0xf9, 0xf9, 0xf9);
    mapSettingsDefauold["legend.enLegend"] = true;
//    mapSettingsDefauold["scale.windowSec"] = 5.0;
//    mapSettingsDefauold["scale.simpleMode"] = false;
//    mapSettingsDefauold["scale.optimize.points"] = true;
//    mapSettingsDefauold["openGL.enOpenGL"] = true;

//    mapSettings = mapSettingsDefauold;
    restoreDefoultSetings();
}

ChartView::ChartView(QChart *chart, QWidget *parent) :
    QChartView(chart, parent)/*, mFactor(1.0)*/
{
    this->setMouseTracking(true);
    this->setRenderHint(QPainter::Antialiasing);
    this->setRubberBand(QChartView::RectangleRubberBand);
    leftButtonPresed = false;
}

void ChartView::wheelEvent(QWheelEvent *event)
{

    qreal factor = event->angleDelta().y() > 0 ? 0.5 : 2;

    QRectF rect = chart()->plotArea();
    QPointF c = chart()->plotArea().center();

    if(!event->modifiers().testFlag(Qt::ControlModifier))
        rect.setWidth(factor*rect.width());
//    else
//        rect.setWidth(rect.width());

    if(!event->modifiers().testFlag(Qt::ShiftModifier))
        rect.setHeight(factor*rect.height());
//    else
//        rect.setHeight(rect.height());

    rect.moveCenter(QPointF(event->pos()));
    chart()->zoomIn(rect);

    QCursor::setPos(this->mapToGlobal(c.toPoint()));

//    qDebug() << /*chart()->mapToValue(event->pos())*/event->pos() << c << rect;

//    qreal factor = event->angleDelta().y() > 0? 0.5: 2.0;
////    chart()->zoom(factor);
////    event->accept();
//    if(factor > 0)
//        chart()->zoomIn();
//    else
//        chart()->zoomOut();

    QChartView::wheelEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    int newRubberBand = NoRubberBand;
    if(event->modifiers().testFlag(Qt::ControlModifier))
        newRubberBand |= VerticalRubberBand;
    if(event->modifiers().testFlag(Qt::ShiftModifier))
        newRubberBand |= HorizontalRubberBand;
    this->setRubberBand((QChartView::RubberBand)newRubberBand);
    if(newRubberBand == NoRubberBand)
    {
        //TODO move gruph
//        if(leftButtonPresed)
//        {
//            QPoint diffMovement = oldPos - event->pos();
//            QRectF rect = chart()->plotArea();
//            rect.moveCenter(rect.center() + QPointF(diffMovement));
//            chart()->zoomIn(rect);
//        }
    }

    oldPos = event->pos();
//    qDebug() << chart()->mapToValue(event->pos());
    QChartView::mouseMoveEvent(event);
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        leftButtonPresed = true;
    }

    if (event->button() == Qt::RightButton)
    {
//        rescale axis
        if(event->modifiers().testFlag(Qt::ControlModifier))
            emit rescaleAxisX();
        if(event->modifiers().testFlag(Qt::ShiftModifier))
            emit rescaleAxisY();
    }

    QChartView::mousePressEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        leftButtonPresed = false;
    }

    QChartView::mouseReleaseEvent(event);
}

ChartViewWidget::ChartViewWidget(SettingsAbstract *settings, QWidget *parent) :
    PlotWidgetAbstract(parent),
    emptyGraphs(true), simpleScaleMode(false), lastTime(0, 0),
    enableOptimizerPoints(true), isOptimizerPoints(false)
{
    plotWidget = new QChart;
    view = new ChartView(plotWidget, this);

    //create axis
    axisX = new QDateTimeAxis;
    axisX->setTickCount(5);
    axisX->setFormat("hh:mm:ss.z");

    axisX->setRange(QDateTime(QDate::currentDate(), QTime(0, 0)), QDateTime(QDate::currentDate(), QTime(0, 1)));
    plotWidget->addAxis(axisX, Qt::AlignBottom);

    axisY = new QValueAxis;
    axisY->setLabelFormat("%0.2f");
    plotWidget->addAxis(axisY, Qt::AlignLeft);


//    view->setDragMode(QGraphicsView::NoDrag);
//    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(view);
    layout->setMargin(0);
    this->setLayout(layout);
//    plotWidget->setAcceptHoverEvents(true);

    rightMousePressed = false;
    firstRedraw = true;

    connect(view, SIGNAL(rescaleAxisX()), this, SLOT(rescaleAxisX()));
    connect(view, SIGNAL(rescaleAxisY()), this, SLOT(rescaleAxisY()));
//    plotWidget->setNoAntialiasingOnDrag(true);
    //settings
    connect(&this->settings, SIGNAL(settingsUpdated()), this, SLOT(settingsChanged()));
    this->settings.setSettings(settings);
}

ChartViewWidget::~ChartViewWidget()
{
    delete plotWidget;
}

void ChartViewWidget::redraw(){
    if (firstRedraw){
        // rescale both axis when first data arrives
        if (!emptyGraphs){
            firstRedraw = false;
            rescaleAxisY();
        }
    }
    //TODO scale Y axis

    quint64 band = axisX->max().toMSecsSinceEpoch() - axisX->min().toMSecsSinceEpoch();
    QDateTime max = QDateTime(QDate::currentDate(), lastTime);
    QDateTime min = QDateTime::fromMSecsSinceEpoch(max.toMSecsSinceEpoch() - band);
    axisX->setRange(min, max);
    plotWidget->update();
}

void ChartViewWidget::addPlot(VarChannel *varChanale)
{
    if(mapPlots.contains(varChanale))//if gpuh alredy maked
        return;

//    QColor lineColor = varChanale->lineColor();
//    QColor trackColor = QColor(255 - lineColor.red(), 255 - lineColor.green(), 255 - lineColor.blue());

    QLineSeries *newGruph = new QLineSeries(this);
    plotWidget->addSeries(newGruph);
    newGruph->setName(varChanale->displayName());
//    newGruph->setLineStyle((QCPGraph::LineStyle)varChanale->lineStyle());
//    newGruph->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)varChanale->dotStyle()));
    newGruph->setPen(QPen(varChanale->lineColor()));
    newGruph->attachAxis(axisX);
    newGruph->attachAxis(axisY);
    // newGruph->setAdaptiveSampling(true); // default on


    connect(varChanale, SIGNAL(updatePlot()), this, SLOT(doUpdatePlot()));
    connect(varChanale, SIGNAL(changePlotColor()), this, SLOT(updateColourPlot()));
    connect(varChanale, SIGNAL(changePlotLineWidth()), this, SLOT(updateLineWidthGruph()));
    connect(varChanale, SIGNAL(changePlotLineStyle()), this, SLOT(updateLineStyleGruph()));
    connect(varChanale, SIGNAL(changePlotDotStyle()), this, SLOT(updateDotStyleGruph()));
    connect(varChanale, SIGNAL(changeDisplayName()), this, SLOT(updateDisplayNameGruph()));
    connect(varChanale, SIGNAL(selectPlot()), this, SLOT(plotSelecting()));
    connect(varChanale, SIGNAL(clearGraph()), this, SLOT(plotClear()));

    mapPlots[varChanale] = newGruph;
//    mapTrackers[varChanale] = newTracker;

    doUpdatePlot(varChanale, newGruph);
    plotWidget->update();

//    plotWidget->axisX()->setRange(varChanale->getValue(0).qtime, varChanale->getValue(varChanale->getBufferSize() - 1).qtime);
//    plotWidget->axisY()->setRange(-200, 200);
}

void ChartViewWidget::deletePlot(VarChannel *varChanale)
{
    if(!mapPlots.contains(varChanale))//if is note created
        return;

    disconnect(varChanale, SIGNAL(updatePlot()), this, SLOT(doUpdatePlot()));
    disconnect(varChanale, SIGNAL(changePlotColor()), this, SLOT(updateColourPlot()));
    disconnect(varChanale, SIGNAL(changePlotLineWidth()), this, SLOT(updateLineWidthGruph()));
    disconnect(varChanale, SIGNAL(changePlotLineStyle()), this, SLOT(updateLineStyleGruph()));
    disconnect(varChanale, SIGNAL(changePlotDotStyle()), this, SLOT(updateDotStyleGruph()));
    disconnect(varChanale, SIGNAL(changeDisplayName()), this, SLOT(updateDisplayNameGruph()));
    disconnect(varChanale, SIGNAL(selectPlot()), this, SLOT(plotSelecting()));
    disconnect(varChanale, SIGNAL(clearGraph()), this, SLOT(plotClear()));

    QLineSeries* gruph = mapPlots[varChanale];
//    QCPItemTracer* tracker = mapTrackers[varChanale];

    mapPlots.remove(varChanale);
//    mapTrackers.remove(varChanale);

    plotWidget->removeSeries(gruph);
//    plotWidget->removeItem(tracker);
    plotWidget->update();
//    plotWidget->replot();

    delete gruph;
}

QLineSeries *ChartViewWidget::getGruph(QObject *_sender, VarChannel** varChanale)
{
    *varChanale = nullptr;
    //get sender of this signal
//    VarChannel * varChanale = qobject_cast<VarChannel*>(_sender);//? it is note work now becouse it show
    if(typeid(*_sender) != typeid(VarChannel))
        return nullptr;

    *varChanale = (VarChannel*)_sender;


    if(!mapPlots.contains(*varChanale))//if is note created
        return nullptr;


    return mapPlots[*varChanale];
}

void ChartViewWidget::restoreStyleGraphs()
{
//    QMap<VarChannel*,QCPGraph*>::const_iterator i = mapPlots.constBegin();
//    while (i != mapPlots.constEnd()) {
//        QCPGraph* graph = i.value();
//        VarChannel* ch = i.key();
//        QPen pen = graph->pen();
//        pen.setWidth(ch->lineWidth());
//        graph->setPen(pen);
//        graph->setScatterStyle((QCPScatterStyle::ScatterShape)ch->dotStyle());
//        ++i;
    //    }
}

void ChartViewWidget::resizeEvent(QResizeEvent *event)
{
    plotWidget->resize(event->size());
}

void ChartViewWidget::doUpdatePlot()
{
    VarChannel* varChanale;
    QLineSeries* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;


    return doUpdatePlot(varChanale, gpuh);
}

void ChartViewWidget::doUpdatePlot(VarChannel *varChanale, QLineSeries* gpuh)
{
    //try just apend data
    if(varChanale->getBufferSize() < gpuh->count())
    {//if replot gruyph
        gpuh->clear();
    }

    for(int i = gpuh->count(); i < varChanale->getBufferSize(); i++)
    {
        VarValue val = varChanale->getValue(i);
        QTime time = val.qtime;
        if(time > lastTime)
            lastTime = time;
//        gpuh->addData(val.time.time_since_epoch().count(), val.value);
        QDateTime dateTiem = QDateTime::currentDateTime();
        dateTiem.setTime(val.qtime);
        gpuh->append(dateTiem.toMSecsSinceEpoch(), val.value);

//        plotWidget->scroll(dateTiem.toMSecsSinceEpoch(), 0);
    }

    if(varChanale->getBufferSize())
        emptyGraphs = false;
}

void ChartViewWidget::updateColourPlot()
{
    VarChannel* varChanale;
    QLineSeries* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;

    QColor lineColor = varChanale->lineColor();
    QColor trackColor = QColor(255 - lineColor.red(), 255 - lineColor.green(), 255 - lineColor.blue());

    QPen pen = gpuh->pen();
    pen.setColor(lineColor);
    gpuh->setPen(pen);

//    mapTrackers[varChanale]->setPen(QPen(trackColor));

    plotWidget->update();
//    plotWidget->replot();
}

void ChartViewWidget::updateLineWidthGruph()
{
    VarChannel* varChanale;
    QLineSeries* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;

    QPen pen = gpuh->pen();
    pen.setWidth(varChanale->lineWidth());
    gpuh->setPen(pen);

    plotWidget->update();
//    plotWidget->replot();
}

void ChartViewWidget::updateLineStyleGruph()
{
    VarChannel* varChanale;
    QLineSeries* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;

//    gpuh->setLineStyle((QCPGraph::LineStyle)varChanale->lineStyle());

    plotWidget->update();
//    plotWidget->replot();
}


void ChartViewWidget::updateDotStyleGruph()
{
    VarChannel* varChanale;
    QLineSeries* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;

//    gpuh->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)varChanale->dotStyle()));

    plotWidget->update();
//    plotWidget->replot();
}

void ChartViewWidget::updateDisplayNameGruph()
{
    VarChannel* varChanale;
    QLineSeries* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;

    gpuh->setName(varChanale->displayName());
    plotWidget->update();
//    plotWidget->replot();
}

void ChartViewWidget::settingsChanged()
{
    QMap<QString, QVariant> map = settings.getSettingsMap();

    plotWidget->legend()->setVisible(map["legend.enLegend"].toBool());


//    plotWidget->xAxis->setBasePen(QPen(map["gColor.varColor"].value<QColor>(), 1));
//    plotWidget->yAxis->setBasePen(QPen(map["gColor.varColor"].value<QColor>(), 1));
//    plotWidget->yAxis2->setBasePen(QPen(map["gColor.varColor"].value<QColor>(), 1));
//    plotWidget->xAxis->setTickPen(QPen(map["gColor.varColor"].value<QColor>(), 1));
//    plotWidget->yAxis->setTickPen(QPen(map["gColor.varColor"].value<QColor>(), 1));
//    plotWidget->yAxis2->setTickPen(QPen(map["gColor.varColor"].value<QColor>(), 1));
//    plotWidget->xAxis->setSubTickPen(QPen(map["gColor.varColor"].value<QColor>(), 1));
//    plotWidget->yAxis->setSubTickPen(QPen(map["gColor.varColor"].value<QColor>(), 1));
//    plotWidget->yAxis2->setSubTickPen(QPen(map["gColor.varColor"].value<QColor>(), 1));
//    plotWidget->xAxis->setTickLabelColor(map["gColor.varColor"].value<QColor>());
//    plotWidget->yAxis->setTickLabelColor(map["gColor.varColor"].value<QColor>());
//    plotWidget->yAxis2->setTickLabelColor(map["gColor.varColor"].value<QColor>());
//    plotWidget->xAxis->grid()->setPen(QPen(map["gColor.gridColor"].value<QColor>(), 1, Qt::DotLine));
//    plotWidget->yAxis->grid()->setPen(QPen(map["gColor.gridColor"].value<QColor>(), 1, Qt::DotLine));
//    plotWidget->yAxis2->grid()->setPen(QPen(map["gColor.gridColor"].value<QColor>(), 1, Qt::DotLine));
//    plotWidget->xAxis->grid()->setSubGridPen(QPen(map["gColor.subGridColor"].value<QColor>(), 1, Qt::DotLine));
//    plotWidget->yAxis->grid()->setSubGridPen(QPen(map["gColor.subGridColor"].value<QColor>(), 1, Qt::DotLine));
//    plotWidget->yAxis2->grid()->setSubGridPen(QPen(map["gColor.subGridColor"].value<QColor>(), 1, Qt::DotLine));
//    QLinearGradient plotGradient;
//    plotGradient.setStart(0, 0);
//    plotGradient.setFinalStop(0, 350);
//    plotGradient.setColorAt(0, map["gColor.bgColor1"].value<QColor>());
//    plotGradient.setColorAt(1, map["gColor.bgColor2"].value<QColor>());
//    plotWidget->setBackground(plotGradient);

//    scaleTime = map["scale.windowSec"].toFloat();
//    simpleScaleMode = map["scale.simpleMode"].toBool();

//    if(map["legend.enLegend"].toBool() != subLayout->visible())
//    {
//        if(map["legend.enLegend"].toBool())
//        {
//            subLayout->setVisible(true);
//            plotWidget->plotLayout()->addElement(0, 1, subLayout);
//        }
//        else
//        {
//            subLayout->setVisible(false);
//            plotWidget->plotLayout()->take(subLayout);
//        }
//    }

//    bool useOpenGl = map["openGL.enOpenGL"].toBool();
//    if(plotWidget->openGl() != useOpenGl)
//        plotWidget->setOpenGl(useOpenGl);
////    plotWidget->legend->setVisible(map["enLegend"].toBool());

//    enableOptimizerPoints = map["scale.optimize.points"].toBool();
//    if(!enableOptimizerPoints && isOptimizerPoints)
//    {
//        restoreStyleGraphs();
//        isOptimizerPoints = false;
//    }

    plotWidget->update();
//    plotWidget->replot();

}

void ChartViewWidget::plotSelecting()
{
    VarChannel* varChanale;
    QLineSeries* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;

//    plotWidget->selectedGraphs();

//    qDebug() << varChanale->displayName();
    //TODO

    plotWidget->update();
//    plotWidget->replot();
}

void ChartViewWidget::plotClear()
{
    VarChannel* varChanale;
    QLineSeries* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;

    gpuh->clear();

    plotWidget->update();
    //    plotWidget->replot();
}

void ChartViewWidget::rescaleAxis(bool isX)
{
    QList<VarChannel*> chanales = mapPlots.keys();
    if(chanales.size() != 0)
    {
        //search firstValues
        bool firstValueIsFound = false;
        QTime minTime = QTime::currentTime();
        QTime maxTime = QTime::currentTime();
        double minValue, maxValue;
        for(int i = 0; i < chanales.size(); i++)
        {
            if(chanales[i]->getBufferSize())
            {
                minTime = chanales[i]->getBuffer().at(0).qtime;
                maxTime = minTime;

                minValue = chanales[i]->getBuffer().at(0).value;
                maxValue = minValue;

                firstValueIsFound = true;
                break;
            }
        }

        //if first value is found mean there is no
        if(firstValueIsFound)
        {
            for(int i = 0; i < chanales.size(); i++)
            {
                for(int j = 0; j < chanales[i]->getBufferSize(); j++)
                {
                    //search min time
                    if(minTime > chanales[i]->getBuffer().at(j).qtime)
                        minTime = chanales[i]->getBuffer().at(j).qtime;
                    if(maxTime < chanales[i]->getBuffer().at(j).qtime)
                        maxTime = chanales[i]->getBuffer().at(j).qtime;

                    //search min values
                    if(minValue > chanales[i]->getBuffer().at(j).value)
                        minValue = chanales[i]->getBuffer().at(j).value;
                    if(maxValue < chanales[i]->getBuffer().at(j).value)
                        maxValue = chanales[i]->getBuffer().at(j).value;
                }
            }

            QDateTime minDataTime = QDateTime::currentDateTime();
            QDateTime maxDataTime = QDateTime::currentDateTime();
            minDataTime.setTime(minTime);
            maxDataTime.setTime(maxTime);

            if(isX)
                axisX->setRange(minDataTime, maxDataTime);
            else
                axisY->setRange(minValue, maxValue);
        }
    }
}

void ChartViewWidget::rescaleAxisX()
{
    rescaleAxis(true);
}

void ChartViewWidget::rescaleAxisY()
{
    rescaleAxis(false);
}
