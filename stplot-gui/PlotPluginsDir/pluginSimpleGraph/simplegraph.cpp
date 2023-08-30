#include "simplegraph.h"
#include "ui_simplegraph.h"
#include <QLayout>
#include "initplot.h"
#include <QDebug>

SimpleGraph::SimpleGraph(QWidget *parent) :
    PlotWidgetAbstract(parent),
    ui(new Ui::SimpleGraph)
{
    ui->setupUi(this);

    plotWidget = new QCustomPlot(this);
    defaultPlotInit(plotWidget);

    QGridLayout *loaout = new QGridLayout(this);
    loaout->addWidget(plotWidget,0, 0, 1, 1);
    this->setLayout(loaout);
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
    newGruph->setName(varChanale->name());


    connect(varChanale, SIGNAL(updatePlot()), this, SLOT(doUpdatePlot()));
    mapPlots[varChanale] = newGruph;
}

void SimpleGraph::deletePlot(VarChannel *varChanale)
{
    if(!mapPlots.contains(varChanale))//if is note created
        return;


    disconnect(varChanale, SIGNAL(updatePlot()), this, SLOT(doUpdatePlot()));
    mapPlots.remove(varChanale);
}

void SimpleGraph::doUpdatePlot()
{
    //get sender of this signal
    VarChannel * varChanale = qobject_cast<VarChannel*>(sender());

    if(!varChanale)
        return;

    if(!mapPlots.contains(varChanale))//if is note created
        return;

    QCPGraph* gpuh = mapPlots[varChanale];

    //try just apend data
    if(varChanale->getTotalSizePlot() < gpuh->dataCount())
    {//if replot gruyph
        QVector<double> zeroAray;
        gpuh->setData(zeroAray, zeroAray);
    }



    for(int i = gpuh->dataCount(); i < varChanale->getTotalSizePlot(); i++)
    {
        VarValue val = varChanale->getValue(i);
        gpuh->addData(val.time.time_since_epoch().count(), val.value);
    }


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
