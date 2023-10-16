#include "simplegraph.h"
#include "ui_simplegraph.h"
#include <QLayout>
#include "initplot.h"
#include <QDebug>

SimpleGraphSettings::SimpleGraphSettings(QObject *parent) : PlotSettingsAbstract(parent)
{
    mapSettingsDefauold["varColor"] = QColor(Qt::white);
    mapSettingsDefauold["gridColor"] = QColor(140, 140, 140);
    mapSettingsDefauold["subGridColor"] = QColor(80, 80, 80);
    mapSettingsDefauold["bgColor1"] = QColor(80, 80, 80);
    mapSettingsDefauold["bgColor2"] = QColor(50, 50, 50);

//    mapSettings = mapSettingsDefauold;
    restoreDefoultSetings();
}

SimpleGraph::SimpleGraph(PlotSettingsAbstract *settings, QWidget *parent) :
    PlotWidgetAbstract(parent),
    ui(new Ui::SimpleGraph)
{
    ui->setupUi(this);

    plotWidget = new QCustomPlot(this);
    defaultPlotInit(plotWidget);

    //time x axis
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s:%z");
    plotWidget->xAxis->setTicker(timeTicker);


    QGridLayout *loaout = new QGridLayout(this);
    loaout->addWidget(plotWidget,0, 0, 1, 1);
    this->setLayout(loaout);

    //settings
    connect(&this->settings, SIGNAL(settingsUpdated()), this, SLOT(settingsChanged()));
    this->settings.setSettings(settings);
}

SimpleGraph::~SimpleGraph()
{
    delete ui;
}

void SimpleGraph::addPlot(VarChannel *varChanale)
{
    if(mapPlots.contains(varChanale))//if gpuh alredy maked
        return;

    QCPGraph* newGruph = plotWidget->addGraph();
    newGruph->setName(varChanale->getName());
    newGruph->setLineStyle((QCPGraph::LineStyle)varChanale->lineStyle());
    newGruph->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)varChanale->dotStyle()));
    newGruph->setPen(QPen(varChanale->lineColor()));

    connect(varChanale, SIGNAL(updatePlot()), this, SLOT(doUpdatePlot()));
    connect(varChanale, SIGNAL(changePlotLineWidth()), this, SLOT(updateColourPlot()));
    connect(varChanale, SIGNAL(changePlotLineWidth()), this, SLOT(updateLineWidthGruph()));
    connect(varChanale, SIGNAL(changePlotLineStyle()), this, SLOT(updateLineStyleGruph()));
    connect(varChanale, SIGNAL(changePlotDotStyle()), this, SLOT(updateDotStyleGruph()));
    mapPlots[varChanale] = newGruph;
}

void SimpleGraph::deletePlot(VarChannel *varChanale)
{
    if(!mapPlots.contains(varChanale))//if is note created
        return;


    disconnect(varChanale, SIGNAL(updatePlot()), this, SLOT(doUpdatePlot()));
    mapPlots.remove(varChanale);
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

    gpuh->rescaleAxes();
    plotWidget->update();
    plotWidget->replot();
}

void SimpleGraph::updateColourPlot()
{
    VarChannel* varChanale;
    QCPGraph* gpuh = getGruph(sender(), &varChanale);

    gpuh->setPen(QPen(varChanale->lineColor()));

    plotWidget->update();
    plotWidget->replot();
}

void SimpleGraph::updateLineWidthGruph()
{
    VarChannel* varChanale;
    QCPGraph* gpuh = getGruph(sender(), &varChanale);
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

    gpuh->setLineStyle((QCPGraph::LineStyle)varChanale->lineStyle());

    plotWidget->update();
    plotWidget->replot();
}


void SimpleGraph::updateDotStyleGruph()
{
    VarChannel* varChanale;
    QCPGraph* gpuh = getGruph(sender(), &varChanale);

    gpuh->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)varChanale->dotStyle()));

    plotWidget->update();
    plotWidget->replot();
}

void SimpleGraph::settingsChanged()
{
    QMap<QString, QVariant> map = settings.getSettingsMap();


    plotWidget->xAxis->setBasePen(QPen(map["varColor"].value<QColor>(), 1));
    plotWidget->yAxis->setBasePen(QPen(map["varColor"].value<QColor>(), 1));
    plotWidget->yAxis2->setBasePen(QPen(map["varColor"].value<QColor>(), 1));
    plotWidget->xAxis->setTickPen(QPen(map["varColor"].value<QColor>(), 1));
    plotWidget->yAxis->setTickPen(QPen(map["varColor"].value<QColor>(), 1));
    plotWidget->yAxis2->setTickPen(QPen(map["varColor"].value<QColor>(), 1));
    plotWidget->xAxis->setSubTickPen(QPen(map["varColor"].value<QColor>(), 1));
    plotWidget->yAxis->setSubTickPen(QPen(map["varColor"].value<QColor>(), 1));
    plotWidget->yAxis2->setSubTickPen(QPen(map["varColor"].value<QColor>(), 1));
    plotWidget->xAxis->setTickLabelColor(map["varColor"].value<QColor>());
    plotWidget->yAxis->setTickLabelColor(map["varColor"].value<QColor>());
    plotWidget->yAxis2->setTickLabelColor(map["varColor"].value<QColor>());
    plotWidget->xAxis->grid()->setPen(QPen(map["gridColor"].value<QColor>(), 1, Qt::DotLine));
    plotWidget->yAxis->grid()->setPen(QPen(map["gridColor"].value<QColor>(), 1, Qt::DotLine));
    plotWidget->yAxis2->grid()->setPen(QPen(map["gridColor"].value<QColor>(), 1, Qt::DotLine));
    plotWidget->xAxis->grid()->setSubGridPen(QPen(map["subGridColor"].value<QColor>(), 1, Qt::DotLine));
    plotWidget->yAxis->grid()->setSubGridPen(QPen(map["subGridColor"].value<QColor>(), 1, Qt::DotLine));
    plotWidget->yAxis2->grid()->setSubGridPen(QPen(map["subGridColor"].value<QColor>(), 1, Qt::DotLine));

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, map["bgColor1"].value<QColor>());
    plotGradient.setColorAt(1, map["bgColor2"].value<QColor>());
    plotWidget->setBackground(plotGradient);

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
