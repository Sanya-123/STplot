#include "simpletable.h"
#include "ui_simpletable.h"
#include <QLayout>
#include "initplot.h"
#include <QDebug>

SimpleTableSettings::SimpleTableSettings(QObject *parent) : PlotSettingsAbstract(parent)
{
    mapSettingsDefauold["gColor.varColor"] = QColor(Qt::white);
//    mapSettingsDefauold["gColor.gridColor"] = QColor(140, 140, 140);
//    mapSettingsDefauold["gColor.subGridColor"] = QColor(80, 80, 80);
//    mapSettingsDefauold["gColor.bgColor1"] = QColor(80, 80, 80);
//    mapSettingsDefauold["gColor.bgColor2"] = QColor(50, 50, 50);

//    mapSettings = mapSettingsDefauold;
    restoreDefoultSetings();
}

SimpleTable::SimpleTable(PlotSettingsAbstract *settings, QWidget *parent) :
    PlotWidgetAbstract(parent),
    ui(new Ui::SimpleTable)
{
    ui->setupUi(this);

    //settings
    connect(&this->settings, SIGNAL(settingsUpdated()), this, SLOT(settingsChanged()));
    this->settings.setSettings(settings);
}

SimpleTable::~SimpleTable()
{
    delete ui;
}

void SimpleTable::addPlot(VarChannel *varChanale)
{
    if(mapPlots.contains(varChanale))//if gpuh alredy maked
        return;

    mapPlots.append(varChanale);

    int rowNumber = ui->tableWidget_table->rowCount();
    ui->tableWidget_table->setRowCount(rowNumber + 1);

    ui->tableWidget_table->setItem(rowNumber, 0, new QTableWidgetItem(varChanale->displayName()));
    ui->tableWidget_table->setItem(rowNumber, 1, new QTableWidgetItem(QString::number(varChanale->getValue())));


    connect(varChanale, SIGNAL(updatePlot()), this, SLOT(doUpdatePlot()));
    connect(varChanale, SIGNAL(changePlotColor()), this, SLOT(updateColourPlot()));
    connect(varChanale, SIGNAL(changePlotLineWidth()), this, SLOT(updateLineWidthGruph()));
    connect(varChanale, SIGNAL(changePlotLineStyle()), this, SLOT(updateLineStyleGruph()));
    connect(varChanale, SIGNAL(changePlotDotStyle()), this, SLOT(updateDotStyleGruph()));
    connect(varChanale, SIGNAL(changeDisplayName()), this, SLOT(updateDisplayNameGruph()));
    connect(varChanale, SIGNAL(selectPlot()), this, SLOT(plotSelecting()));

    mapPlots.append(varChanale);

//    doUpdatePlot(varChanale, newGruph);
}

void SimpleTable::deletePlot(VarChannel *varChanale)
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


    int numberRow = mapPlots.indexOf(varChanale);
    ui->tableWidget_table->removeRow(numberRow);

    mapPlots.remove(numberRow);
}

int SimpleTable::getGruph(QObject *_sender, VarChannel** varChanale)
{
    *varChanale = nullptr;
    //get sender of this signal
//    VarChannel * varChanale = qobject_cast<VarChannel*>(_sender);//? it is note work now becouse it show
    if(typeid(*_sender) != typeid(VarChannel))
        return -1;

    *varChanale = (VarChannel*)_sender;


    if(!mapPlots.contains(*varChanale))//if is note created
        return -1;


    return mapPlots.indexOf(*varChanale);
}

void SimpleTable::redraw(){

}

void SimpleTable::doUpdatePlot()
{
    VarChannel* varChanale;
    int gpuh = getGruph(sender(), &varChanale);
    if(gpuh == -1 || varChanale == nullptr)
        return;

    ui->tableWidget_table->item(gpuh, 1)->setText(QString::number(varChanale->getValue()));

//    return doUpdatePlot(varChanale, gpuh);
}

//void SimpleTable::doUpdatePlot(VarChannel *varChanale, QCPGraph* gpuh)
//{
//    //try just apend data
//    if(varChanale->getBufferSize() < gpuh->dataCount())
//    {//if replot gruyph
//        QVector<double> zeroAray;
//        gpuh->setData(zeroAray, zeroAray);
//    }

//    for(int i = gpuh->dataCount(); i < varChanale->getBufferSize(); i++)
//    {
//        VarValue val = varChanale->getValue(i);
////        gpuh->addData(val.time.time_since_epoch().count(), val.value);
//        gpuh->addData(val.qtime.msecsSinceStartOfDay()*1.0/1000.0, val.value);
//    }
//}

void SimpleTable::updateColourPlot()
{
    VarChannel* varChanale;
    int gpuh = getGruph(sender(), &varChanale);
    if(gpuh == -1 || varChanale == nullptr)
        return;

//    QColor lineColor = varChanale->lineColor();
//    QColor trackColor = QColor(255 - lineColor.red(), 255 - lineColor.green(), 255 - lineColor.blue());

//    gpuh->setPen(QPen(lineColor));
}

void SimpleTable::updateLineWidthGruph()
{
    VarChannel* varChanale;
    int gpuh = getGruph(sender(), &varChanale);
    if(gpuh == -1 || varChanale == nullptr)
        return;

//    QPen pen = QPen(varChanale->lineColor());
//    pen.setWidth(varChanale->lineWidth());
//    gpuh->setPen(pen);
}

void SimpleTable::updateLineStyleGruph()
{
    VarChannel* varChanale;
    int gpuh = getGruph(sender(), &varChanale);
    if(gpuh == -1 || varChanale == nullptr)
        return;

//    gpuh->setLineStyle((QCPGraph::LineStyle)varChanale->lineStyle());
}

void SimpleTable::updateDotStyleGruph()
{
    VarChannel* varChanale;
    int gpuh = getGruph(sender(), &varChanale);
    if(gpuh == -1 || varChanale == nullptr)
        return;

//    gpuh->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)varChanale->dotStyle()));
}

void SimpleTable::updateDisplayNameGruph()
{
    VarChannel* varChanale;
    int gpuh = getGruph(sender(), &varChanale);
    if(gpuh == -1 || varChanale == nullptr)
        return;

    ui->tableWidget_table->item(gpuh, 0)->setText(varChanale->displayName());
}

void SimpleTable::settingsChanged()
{
    QMap<QString, QVariant> map = settings.getSettingsMap();


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
////    plotWidget->legend->setVisible(map["enLegend"].toBool());

//    plotWidget->update();
//    plotWidget->replot();

}

void SimpleTable::plotSelecting()
{
    VarChannel* varChanale;
    int gpuh = getGruph(sender(), &varChanale);
    if(gpuh == -1 || varChanale == nullptr)
        return;
}
