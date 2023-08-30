#include "channelmodel.h"
#include <QDebug>
#include "qcustomplot.h"

#define GRUPH_FERST_COLUMN          3

ChannelModel::ChannelModel( QVector<VarChannel*> *channels, QObject *parent)
    : QAbstractTableModel{parent}, numberGraph(0)
{
    m_channels = channels;
}

//Qt::ItemFlags ChannelModel::flags(const QModelIndex &index) const{
//    return Qt::ItemFlags();
//}

int ChannelModel::rowCount(const QModelIndex &parent) const
{
    return m_channels->size();
}

int ChannelModel::columnCount(const QModelIndex &parent) const
{
    return GRUPH_FERST_COLUMN + numberGraph;
}

QVariant ChannelModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0){
            return QVariant( m_channels->at(index.row())->name());
        }
        else if(index.column() == 1){
            return QVariant("0x" + QString::number(m_channels->at(index.row())->addres(), 16).rightJustified(8, '0'));
        }
        else
            return QVariant("");
    }
    else if (role == Qt::CheckStateRole)
    {
        //set column graph as checke
        if(index.column() >= GRUPH_FERST_COLUMN)
        {
            int numGraph = index.column() - GRUPH_FERST_COLUMN;
            if(numGraph >= numberGraph)
                return QVariant();

            //if varible is new you should reinit size gruph
            if(m_channels->at(index.row())->getTotalSizePlot() != numberGraph)
            {
                m_channels->at(index.row())->setTotalSizePlot(0);
                m_channels->at(index.row())->setTotalSizePlot(numberGraph);
            }

            return m_channels->at(index.row())->isEnableOnPlot(index.column() - GRUPH_FERST_COLUMN) ? Qt::Checked : Qt::Unchecked;
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if(index.column() == 2)
            return QColor(Qt::green);
    }


    return QVariant();
}

QVariant ChannelModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
        if (section == 0){
            return QVariant("Name");
        }
        if (section == 1){
            return QVariant("Addres");
        }
        else if(section == 2)
        {
            return QVariant("LineColor");
        }
        else if((section - GRUPH_FERST_COLUMN) <  numberGraph)
        {
            return QVariant(graphNames[section - GRUPH_FERST_COLUMN]);
        }
    }
    return QVariant();
}

bool ChannelModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && index.isValid())
    {
        if(index.column() >= GRUPH_FERST_COLUMN)
        {
//            qDebug() << index.column() << index.row() << value;
            m_channels->at(index.row())->setEnableOnPlot(index.column() - GRUPH_FERST_COLUMN, value.toInt());
            emit changeEnablePlo(m_channels->at(index.row()), index.column() - GRUPH_FERST_COLUMN, value.toInt());

            QModelIndex topLeft = index;
            QModelIndex bottomRight = index;
            emit dataChanged(topLeft, bottomRight);

            return true;
        }
    }

    return QAbstractTableModel::setData(index, value, role);
}

Qt::ItemFlags ChannelModel::flags(const QModelIndex &index) const
{
    if (!index.isValid() || index.column() >= (numberGraph + GRUPH_FERST_COLUMN))
        return QAbstractTableModel::flags(index);

    Qt::ItemFlags flags = QAbstractTableModel::flags(index);

    if (index.column() >= GRUPH_FERST_COLUMN)
        return /*flags | */Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return flags;
}

ComboBoxDelegate *ChannelModel::makeIteamLineStye(QObject *parent)
{
    QVector<QCPGraph::LineStyle> shapes;
    shapes << QCPGraph::lsNone;
    shapes << QCPGraph::lsLine;
    shapes << QCPGraph::lsStepLeft;
    shapes << QCPGraph::lsStepRight;
    shapes << QCPGraph::lsStepCenter;
    shapes << QCPGraph::lsImpulse;

    QStringList lineStyles;

    //get name for each style
    for(int i = 0; i < shapes.size(); i++)
    {
        QString name = QCPGraph::staticMetaObject.enumerator(
                    QCPGraph::staticMetaObject.indexOfEnumerator("LineStyle")).valueToKey(shapes.at(i));

        lineStyles.append(name);
    }

    return new ComboBoxDelegate(lineStyles, parent);
}

ComboBoxDelegate *ChannelModel::makeIteamDotStye(QObject *parent)
{
    //https://www.qcustomplot.com/index.php/demos/scatterstyledemo
    QVector<QCPScatterStyle::ScatterShape> shapes;
    shapes << QCPScatterStyle::ssNone;
    shapes << QCPScatterStyle::ssDot;
    shapes << QCPScatterStyle::ssCross;
    shapes << QCPScatterStyle::ssPlus;
    shapes << QCPScatterStyle::ssCircle;
    shapes << QCPScatterStyle::ssDisc;
    shapes << QCPScatterStyle::ssSquare;
    shapes << QCPScatterStyle::ssDiamond;
    shapes << QCPScatterStyle::ssStar;
    shapes << QCPScatterStyle::ssTriangle;
    shapes << QCPScatterStyle::ssTriangleInverted;
    shapes << QCPScatterStyle::ssCrossSquare;
    shapes << QCPScatterStyle::ssPlusSquare;
    shapes << QCPScatterStyle::ssCrossCircle;
    shapes << QCPScatterStyle::ssPlusCircle;
    shapes << QCPScatterStyle::ssPeace;

    QStringList dotStyles;

    //get name for each style
    for(int i = 0; i < shapes.size(); i++)
    {
        QString name = QCPScatterStyle::staticMetaObject.enumerator(
                    QCPScatterStyle::staticMetaObject.indexOfEnumerator("ScatterShape")).valueToKey(shapes.at(i));

        dotStyles.append(name);
    }

    return new ComboBoxDelegate(dotStyles, parent);
}

void ChannelModel::addPlot()
{
    numberGraph++;
    graphNames.append("_");

    for(int i = 0; i < m_channels->size(); i++)
    {
        m_channels->at(i)->setTotalSizePlot(numberGraph);
    }

    emit updateViewport();
    emit layoutChanged();
}

void ChannelModel::deletePlot(int number)
{
    if(number < numberGraph)
    {
        //update grupt list for each varible
        for(int i = 0; i < m_channels->size(); i++)
        {
            if(m_channels->at(i)->getTotalSizePlot() == numberGraph)
            {
                m_channels->at(i)->removePlot(number);
//                plotList->removeAt(number);
            }//if size != numberGraph it is mean that there is error in var list
            else
            {
                m_channels->at(i)->setTotalSizePlot(0);
                m_channels->at(i)->setTotalSizePlot(numberGraph);
            }

        }

        numberGraph--;
        graphNames.removeAt(number);
        emit updateViewport();
        emit layoutChanged();
    }
}

void ChannelModel::setPlotName(int number, QString name)
{
    if(number < numberGraph)
    {
        graphNames[number] = name;
        emit updateViewport();
        emit layoutChanged();
    }
}
