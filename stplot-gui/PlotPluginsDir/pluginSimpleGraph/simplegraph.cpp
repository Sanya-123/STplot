#include "simplegraph.h"
#include "ui_simplegraph.h"

SimpleGraph::SimpleGraph(QWidget *parent) :
    PlotWidgetAbstract(parent),
    ui(new Ui::SimpleGraph)
{
    ui->setupUi(this);
}

SimpleGraph::~SimpleGraph()
{
    delete ui;
}

void SimpleGraph::addPlot(QString plotName)
{

}

void SimpleGraph::deletePlot(QString plotName)
{

}

bool SimpleGraph::plotVar(QString plotName, QVector<VarValue> values)
{
    return true;
}

void SimpleGraph::setName(QString name)
{
    this->name = name;
}

QString SimpleGraph::getName()
{
    return name;
}
