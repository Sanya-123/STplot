#include "simpletable.h"
#include "ui_simpletable.h"
#include <QLayout>
//#include "initplot.h"
#include <QDebug>

SimpleTableSettings::SimpleTableSettings(QObject *parent) : SettingsAbstract(parent)
{
    mapSettingsDefauold["gColor.enColor"] = false;
    mapSettingsDefauold["gColor.foregroung"] = QColor(Qt::black);
    mapSettingsDefauold["gColor.backgroung"] = QColor(Qt::white);
//    mapSettingsDefauold["gColor.gridColor"] = QColor(140, 140, 140);
//    mapSettingsDefauold["gColor.subGridColor"] = QColor(80, 80, 80);
//    mapSettingsDefauold["gColor.bgColor1"] = QColor(80, 80, 80);
//    mapSettingsDefauold["gColor.bgColor2"] = QColor(50, 50, 50);

//    mapSettings = mapSettingsDefauold;
    restoreDefoultSetings();
}


void SimpleTable::resizeEvent(QResizeEvent *event){
    ui->tableWidget_table->setColumnWidth(0,this->size().width()*0.7);
    ui->tableWidget_table->setColumnWidth(1,this->size().width()*0.3);
}

SimpleTable::SimpleTable(SettingsAbstract *settings, QWidget *parent) :
    PlotWidgetAbstract(parent),
    ui(new Ui::SimpleTable)
{
    ui->setupUi(this);

    ui->tableWidget_table->setColumnWidth(0,this->size().width()*0.7);
    ui->tableWidget_table->setColumnWidth(1,this->size().width()*0.3);
//    connect(ui->tableWidget_table, SIGNAL(cellActivated(int,int)), this, SLOT(updateCellValues(int,int)));
    connect(ui->tableWidget_table->itemDelegate(), SIGNAL(commitData(QWidget*)), this, SLOT(updateCellValues()));

    connect(ui->horizontalScrollBar_delay_offset, SIGNAL(valueChanged(int)), this, SLOT(updatePositionScroll(int)));


    // ui->tableWidget_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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

    ui->tableWidget_table->item(rowNumber, 0)->setFlags(ui->tableWidget_table->item(rowNumber, 0)->flags() & ~(Qt::ItemIsEditable));

    if(enColor)
    {
        ui->tableWidget_table->item(rowNumber, 0)->setForeground(varChanale->lineColor());
        ui->tableWidget_table->item(rowNumber, 1)->setForeground(varChanale->lineColor());
    }
    else
    {
        ui->tableWidget_table->item(rowNumber, 0)->setForeground(colorForeground);
        ui->tableWidget_table->item(rowNumber, 1)->setForeground(colorForeground);
    }

    ui->tableWidget_table->item(rowNumber, 0)->setBackground(colorBackground);
    ui->tableWidget_table->item(rowNumber, 1)->setBackground(colorBackground);



    connect(varChanale, SIGNAL(updatePlot()), this, SLOT(doUpdatePlot()));
    connect(varChanale, SIGNAL(changePlotColor()), this, SLOT(updateColourPlot()));
    connect(varChanale, SIGNAL(changePlotLineWidth()), this, SLOT(updateLineWidthGruph()));
    connect(varChanale, SIGNAL(changePlotLineStyle()), this, SLOT(updateLineStyleGruph()));
    connect(varChanale, SIGNAL(changePlotDotStyle()), this, SLOT(updateDotStyleGruph()));
    connect(varChanale, SIGNAL(changeDisplayName()), this, SLOT(updateDisplayNameGruph()));
    connect(varChanale, SIGNAL(selectPlot()), this, SLOT(plotSelecting()));
    connect(varChanale, SIGNAL(clearGraph()), this, SLOT(clearPlot()));

    // mapPlots.append(varChanale);

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
    disconnect(varChanale, SIGNAL(clearGraph()), this, SLOT(clearPlot()));


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

    if((varChanale->getBufferSize() - 1) > (-ui->horizontalScrollBar_delay_offset->minimum()))
        ui->horizontalScrollBar_delay_offset->setMinimum(-(varChanale->getBufferSize() - 1));

    if(!ui->tableWidget_table->isPersistentEditorOpen(ui->tableWidget_table->item(gpuh, 1)))
    {
        ui->tableWidget_table->item(gpuh, 1)->setText(QString::number(varChanale->getValue()));
    }

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

    if(!enColor)
        return;

    ui->tableWidget_table->item(gpuh, 0)->setForeground(varChanale->lineColor());
    ui->tableWidget_table->item(gpuh, 1)->setForeground(varChanale->lineColor());

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

    enColor = map["gColor.enColor"].toBool();
    colorForeground = map["gColor.foregroung"].value<QColor>();
    colorBackground = map["gColor.backgroung"].value<QColor>();

    for(int i = 0; i < mapPlots.size(); i++)
    {
        QColor color = enColor ? mapPlots[i]->lineColor() : colorForeground;
        ui->tableWidget_table->item(i, 0)->setForeground(color);
        ui->tableWidget_table->item(i, 1)->setForeground(color);

        ui->tableWidget_table->item(i, 0)->setBackground(colorBackground);
        ui->tableWidget_table->item(i, 1)->setBackground(colorBackground);
    }

}

void SimpleTable::plotSelecting()
{
    VarChannel* varChanale;
    int gpuh = getGruph(sender(), &varChanale);
    if(gpuh == -1 || varChanale == nullptr)
        return;

    ui->tableWidget_table->selectRow(gpuh);
    QHash<QString, QVariant> props;
    props["channelSelected"] = QVariant(varChanale->getName());
    emit viewPropsUpdated(props);
}

void SimpleTable::clearPlot()
{
    VarChannel* varChanale;
    int gpuh = getGruph(sender(), &varChanale);
    if(gpuh == -1 || varChanale == nullptr)
        return;

    ui->tableWidget_table->item(gpuh, 1)->setText("0");

    //fing graph with maximum bufer size
    int maximumRange = 0;
    for(int i = 0; i < mapPlots.size(); i++)
    {
        //spik cleared gruph
        if(mapPlots[i] == varChanale)
            continue;

        maximumRange = qMax(maximumRange, mapPlots[i]->getBufferSize());
    }

    ui->horizontalScrollBar_delay_offset->setMinimum(-(maximumRange - 1));

}

void SimpleTable::updateCellValues(int row, int column)
{
    qDebug() << row << column;
}

void SimpleTable::updateCellValues()
{
    int numberChanale = ui->tableWidget_table->currentRow();
//    qDebug() << ui->tableWidget_table->currentRow() << ui->tableWidget_table->currentColumn();
    //exec write data
    if(numberChanale < mapPlots.size())
    {
        mapPlots[numberChanale]->writeValues(ui->tableWidget_table->item(numberChanale, 1)->text().toFloat());
    }
}

void SimpleTable::updatePositionScroll(int val)
{
    //NOTE sliber max value is 1 instead of 0
    val = qMax(0, -val);//inver potition becouse scroll is from gegative to positive
    //get last data for each chanales
    for(int i = 0; i < mapPlots.size(); i++)
    {
        double value = mapPlots[i]->getValue(mapPlots[i]->getBufferSize() - val - 1).value;
        ui->tableWidget_table->item(i, 1)->setText(QString::number(value));
    }
}
void SimpleTable::setViewProps(QHash<QString, QVariant> props)
{
    // double rangeUpper = props["timeRangeUpper"].toDouble();
    // double rangeLower = props["timeRangeLower"].toDouble();
    int mouseIndex = props["mouseDataIndex"].toInt();
    for(int i = 0; i < mapPlots.size(); i++)
    {
        double value = mapPlots[i]->getValue(mouseIndex).value;
        ui->tableWidget_table->item(i, 1)->setText(QString::number(value));
    }
}
