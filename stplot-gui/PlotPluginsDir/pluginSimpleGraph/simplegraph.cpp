#include "simplegraph.h"
#include "ui_simplegraph.h"
#include <QLayout>
#include "initplot.h"
#include <QDebug>

SimpleGraphSettings::SimpleGraphSettings(QObject *parent) : PlotSettingsAbstract(parent)
{
    mapSettingsDefauold["gColor.varColor"] = QColor(Qt::white);
    mapSettingsDefauold["gColor.gridColor"] = QColor(140, 140, 140);
    mapSettingsDefauold["gColor.subGridColor"] = QColor(80, 80, 80);
    mapSettingsDefauold["gColor.bgColor1"] = QColor(80, 80, 80);
    mapSettingsDefauold["gColor.bgColor2"] = QColor(50, 50, 50);
    mapSettingsDefauold["legend.enLegend"] = false;
    mapSettingsDefauold["scale.windowSec"] = 5.0;

//    mapSettings = mapSettingsDefauold;
    restoreDefoultSetings();
}

SimpleGraph::SimpleGraph(PlotSettingsAbstract *settings, QWidget *parent) :
    PlotWidgetAbstract(parent),
    ui(new Ui::SimpleGraph), emptyGraphs(true)
{
    ui->setupUi(this);

    plotWidget = new QCustomPlot(this);
    defaultPlotInit(plotWidget);
    plotWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables  | QCP::iMultiSelect | QCP::iSelectLegend);

    //legeng
//    legendValues = new QCPTextElement(plotWidget, "test");
    plotWidget->legend->setVisible(true);
    subLayout = new QCPLayoutGrid;
    plotWidget->plotLayout()->addElement(0, 1, subLayout);
//    subLayout->addElement(0, 0, new QCPLayoutElement);
    subLayout->addElement(0, 0, plotWidget->legend);
    subLayout->addElement(1, 0, new QCPLayoutElement);
//    subLayout->addElement(2, 0, legendValues);
    subLayout->setRowStretchFactor(0, 0.01); // make legend cell (in row 0) take up as little vertical space as possible

    plotWidget->plotLayout()->setColumnStretchFactor(1, 0.001);
    plotWidget->legend->setSelectableParts(QCPLegend::spItems);

    subLayout->setVisible(false);
    plotWidget->plotLayout()->take(subLayout);

    rightMousePressed = false;
    firstRedraw = false;

    //time x axis
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s:%z");
    plotWidget->xAxis->setTicker(timeTicker);

    cursor = new QCPItemLine(plotWidget);
    cursor->setVisible(false);
    cursor->setPen(QPen(QColor(111,111,111)));

    QGridLayout *loaout = new QGridLayout(this);
    loaout->addWidget(plotWidget,0, 0, 1, 1);
    this->setLayout(loaout);

    connect(plotWidget, &QCustomPlot::mouseMove,    this, &SimpleGraph::handleMouseMove);
    connect(plotWidget, &QCustomPlot::mouseRelease, this, &SimpleGraph::handleMouseRelease);
    connect(plotWidget, &QCustomPlot::mousePress,   this, &SimpleGraph::handleMousePress);
    connect(plotWidget->xAxis, SIGNAL(rangeChanged(const QCPRange&,const QCPRange&)), this, SLOT(limitTimeRange(const QCPRange&,const QCPRange&)));

    //settings
    connect(&this->settings, SIGNAL(settingsUpdated()), this, SLOT(settingsChanged()));
    this->settings.setSettings(settings);
}

SimpleGraph::~SimpleGraph()
{
    delete ui;
}

void SimpleGraph::redraw(){
    if (!firstRedraw){
        plotWidget->rescaleAxes(!emptyGraphs);
        if (!emptyGraphs){
            firstRedraw = true;
        }
    }
    else{
        plotWidget->xAxis->rescale();
    }
    // plotWidget->update();
    plotWidget->replot(QCustomPlot::rpQueuedReplot);
}

void SimpleGraph::addPlot(VarChannel *varChanale)
{
    if(mapPlots.contains(varChanale))//if gpuh alredy maked
        return;

    QColor lineColor = varChanale->lineColor();
    QColor trackColor = QColor(255 - lineColor.red(), 255 - lineColor.green(), 255 - lineColor.blue());

    QCPGraph* newGruph = plotWidget->addGraph();
    newGruph->setName(varChanale->displayName() + "\n0");
    newGruph->setLineStyle((QCPGraph::LineStyle)varChanale->lineStyle());
    newGruph->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)varChanale->dotStyle()));
    newGruph->setPen(QPen(varChanale->lineColor()));

    QCPItemTracer* newTracker = new QCPItemTracer(plotWidget);
//    plotWidget->addItem(newTracker);
    newTracker->setGraph(newGruph);
//    newTracker->setGraphKey(7);
    newTracker->setInterpolating(false);
    newTracker->setStyle(QCPItemTracer::tsCircle);
    newTracker->setPen(QPen(trackColor));
//    newTracker->setBrush(Qt::red);
    newTracker->setSize(15);

    connect(varChanale, SIGNAL(updatePlot()), this, SLOT(doUpdatePlot()));
    connect(varChanale, SIGNAL(changePlotColor()), this, SLOT(updateColourPlot()));
    connect(varChanale, SIGNAL(changePlotLineWidth()), this, SLOT(updateLineWidthGruph()));
    connect(varChanale, SIGNAL(changePlotLineStyle()), this, SLOT(updateLineStyleGruph()));
    connect(varChanale, SIGNAL(changePlotDotStyle()), this, SLOT(updateDotStyleGruph()));
    connect(varChanale, SIGNAL(changeDisplayName()), this, SLOT(updateDisplayNameGruph()));
    connect(varChanale, SIGNAL(selectPlot()), this, SLOT(plotSelecting()));

    mapPlots[varChanale] = newGruph;
    mapTrackers[varChanale] = newTracker;

    doUpdatePlot(varChanale, newGruph);
}

void SimpleGraph::deletePlot(VarChannel *varChanale)
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

    QCPGraph* gruph = mapPlots[varChanale];
    QCPItemTracer* tracker = mapTrackers[varChanale];

    mapPlots.remove(varChanale);
    mapTrackers.remove(varChanale);

    plotWidget->removeGraph(gruph);
    plotWidget->removeItem(tracker);
    plotWidget->update();
    plotWidget->replot();
}

QCPGraph *SimpleGraph::getGruph(QObject *_sender, VarChannel** varChanale)
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

void SimpleGraph::doUpdatePlot()
{
    VarChannel* varChanale;
    QCPGraph* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;


    return doUpdatePlot(varChanale, gpuh);
}

void SimpleGraph::doUpdatePlot(VarChannel *varChanale, QCPGraph* gpuh)
{
    //try just apend data
    if(varChanale->getBufferSize() < gpuh->dataCount())
    {//if replot gruyph
        QVector<double> zeroAray;
        gpuh->setData(zeroAray, zeroAray);
    }

    for(int i = gpuh->dataCount(); i < varChanale->getBufferSize(); i++)
    {
        VarValue val = varChanale->getValue(i);
//        gpuh->addData(val.time.time_since_epoch().count(), val.value);
        gpuh->addData(val.qtime.msecsSinceStartOfDay()*1.0/1000.0, val.value);
    }

    if(varChanale->getBufferSize())
        emptyGraphs = false;
}


void SimpleGraph::limitTimeRange(const QCPRange & newRange, const QCPRange & oldRange){
    plotWidget->xAxis->setRange(newRange.upper, scaleTime, Qt::AlignmentFlag::AlignRight);
}


void SimpleGraph::updateColourPlot()
{
    VarChannel* varChanale;
    QCPGraph* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;

    QColor lineColor = varChanale->lineColor();
    QColor trackColor = QColor(255 - lineColor.red(), 255 - lineColor.green(), 255 - lineColor.blue());

    gpuh->setPen(QPen(lineColor));

//    mapTrackers[varChanale]->setPen(QPen(trackColor));

    plotWidget->update();
    plotWidget->replot();
}

void SimpleGraph::updateLineWidthGruph()
{
    VarChannel* varChanale;
    QCPGraph* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;

    QPen pen = QPen(varChanale->lineColor());
    pen.setWidth(varChanale->lineWidth());
    gpuh->setPen(pen);

    plotWidget->update();
    plotWidget->replot();
}

void SimpleGraph::updateLineStyleGruph()
{
    VarChannel* varChanale;
    QCPGraph* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;

    gpuh->setLineStyle((QCPGraph::LineStyle)varChanale->lineStyle());

    plotWidget->update();
    plotWidget->replot();
}


void SimpleGraph::updateDotStyleGruph()
{
    VarChannel* varChanale;
    QCPGraph* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;

    gpuh->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)varChanale->dotStyle()));

    plotWidget->update();
    plotWidget->replot();
}

void SimpleGraph::updateDisplayNameGruph()
{
    VarChannel* varChanale;
    QCPGraph* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;

    gpuh->setName(varChanale->displayName() + "\n0");
    plotWidget->update();
    plotWidget->replot();
}

void SimpleGraph::settingsChanged()
{
    QMap<QString, QVariant> map = settings.getSettingsMap();


    plotWidget->xAxis->setBasePen(QPen(map["gColor.varColor"].value<QColor>(), 1));
    plotWidget->yAxis->setBasePen(QPen(map["gColor.varColor"].value<QColor>(), 1));
    plotWidget->yAxis2->setBasePen(QPen(map["gColor.varColor"].value<QColor>(), 1));
    plotWidget->xAxis->setTickPen(QPen(map["gColor.varColor"].value<QColor>(), 1));
    plotWidget->yAxis->setTickPen(QPen(map["gColor.varColor"].value<QColor>(), 1));
    plotWidget->yAxis2->setTickPen(QPen(map["gColor.varColor"].value<QColor>(), 1));
    plotWidget->xAxis->setSubTickPen(QPen(map["gColor.varColor"].value<QColor>(), 1));
    plotWidget->yAxis->setSubTickPen(QPen(map["gColor.varColor"].value<QColor>(), 1));
    plotWidget->yAxis2->setSubTickPen(QPen(map["gColor.varColor"].value<QColor>(), 1));
    plotWidget->xAxis->setTickLabelColor(map["gColor.varColor"].value<QColor>());
    plotWidget->yAxis->setTickLabelColor(map["gColor.varColor"].value<QColor>());
    plotWidget->yAxis2->setTickLabelColor(map["gColor.varColor"].value<QColor>());
    plotWidget->xAxis->grid()->setPen(QPen(map["gColor.gridColor"].value<QColor>(), 1, Qt::DotLine));
    plotWidget->yAxis->grid()->setPen(QPen(map["gColor.gridColor"].value<QColor>(), 1, Qt::DotLine));
    plotWidget->yAxis2->grid()->setPen(QPen(map["gColor.gridColor"].value<QColor>(), 1, Qt::DotLine));
    plotWidget->xAxis->grid()->setSubGridPen(QPen(map["gColor.subGridColor"].value<QColor>(), 1, Qt::DotLine));
    plotWidget->yAxis->grid()->setSubGridPen(QPen(map["gColor.subGridColor"].value<QColor>(), 1, Qt::DotLine));
    plotWidget->yAxis2->grid()->setSubGridPen(QPen(map["gColor.subGridColor"].value<QColor>(), 1, Qt::DotLine));
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, map["gColor.bgColor1"].value<QColor>());
    plotGradient.setColorAt(1, map["gColor.bgColor2"].value<QColor>());
    plotWidget->setBackground(plotGradient);

    scaleTime = map["scale.windowSec"].toFloat();

    if(map["legend.enLegend"].toBool() != subLayout->visible())
    {
        if(map["legend.enLegend"].toBool())
        {
            subLayout->setVisible(true);
            plotWidget->plotLayout()->addElement(0, 1, subLayout);
        }
        else
        {
            subLayout->setVisible(false);
            plotWidget->plotLayout()->take(subLayout);
        }
    }
//    plotWidget->legend->setVisible(map["enLegend"].toBool());

    plotWidget->update();
    plotWidget->replot();

}

void SimpleGraph::plotSelecting()
{
    VarChannel* varChanale;
    QCPGraph* gpuh = getGruph(sender(), &varChanale);
    if(gpuh == nullptr || varChanale == nullptr)
        return;

    plotWidget->selectedGraphs();

//    qDebug() << varChanale->displayName();
    //TODO

    plotWidget->update();
    plotWidget->replot();
}

void SimpleGraph::handleMouseRelease(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton){
        rightMousePressed = false;
        qDebug() << "RMB released";
    }
}

void SimpleGraph::handleMousePress(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton){
        qDebug() << "RMB pressed";
        rightMousePressed = true;
        lastClickPos = event->pos();
        lastScaleTime = plotWidget->xAxis->range().size();
        valueRangeLower = plotWidget->yAxis->range().lower;
        valueRangeUpper = plotWidget->yAxis->range().upper;
    }
    else if (event->button() == Qt::LeftButton){
        if (event->modifiers().testFlag(Qt::ControlModifier)){
            qDebug() << "Ctrl+LMB pressed";
            plotWidget->rescaleAxes();
        }
    }
}



void SimpleGraph::handleMouseMove(QMouseEvent *event)
{
    // handle right click scaling
    if (rightMousePressed){
        scaleTime = lastScaleTime  + ((lastClickPos.x() - event->pos().x()) * 0.1);
        if (scaleTime < 0.1){
            scaleTime = 0.1;
        }
        plotWidget->xAxis->rescale();

        QCPRange newRange;
        double last_y = plotWidget->yAxis->pixelToCoord(lastClickPos.y());
        double new_y = plotWidget->yAxis->pixelToCoord(event->pos().y());
        newRange.lower = valueRangeLower + ((last_y - new_y) * 1.0);
        newRange.upper = valueRangeUpper - ((last_y - new_y) * 1.0);
        plotWidget->yAxis->setRange(newRange);
        qDebug() << "Val range: " << newRange;
    }

    if(subLayout->visible()){
        // legend enabled

        double x = plotWidget->xAxis->pixelToCoord(event->pos().x());
    //    double y = plotWidget->yAxis->pixelToCoord(event->pos().y());
        double startPos = plotWidget->yAxis->pixelToCoord(0);
        double endPos = plotWidget->yAxis->pixelToCoord(plotWidget->size().height());

        cursor->setVisible(true);
        cursor->start->setCoords(x, startPos);
        cursor->end->setCoords(x, endPos);

        QList<VarChannel*> plots = mapPlots.keys();
    //    QList<QCPItemTracer*> trackers = mapTrackers.values();
        for(int i = 0 ; i < plots.size(); i++)
        {
    //        double _x = x;
            mapTrackers[plots[i]]->setGraphKey(x);
            mapTrackers[plots[i]]->updatePosition();
            mapPlots[plots[i]]->setName(plots[i]->displayName() + "\n" + QString::number(mapTrackers[plots[i]]->position->value()));
        }
    }
    plotWidget->update();
    plotWidget->replot();

}

//bool SimpleGraph::plotVar(QString plotName, QVector<VarValue> values)
//{
//    return true;
//}

//void SimpleGraph::setName(QString name)
//{
//    this->name = name;
//}

//QString SimpleGraph::getName()
//{
//    return name;
//}
