#include "channelmodel.h"
#include <QDebug>

#define GRUPH_FERST_COLUMN          2

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
    return 2 + numberGraph;
}

QVariant ChannelModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0){
            QString* name = m_channels->at(index.row())->name();
            return QVariant(*name);
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

            QVector<bool> * plotList = m_channels->at(index.row())->getPlotList();

            if(plotList->size() != numberGraph)
            {//if there isa proble in gruph size it is mean that it is new varibel
                plotList->resize(0);
                plotList->resize(numberGraph);
            }

            return Qt::Checked;
        }
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
    {//TODO
        if(index.column() >= GRUPH_FERST_COLUMN)
        qDebug() << index.column() << index.row() << value;

        QModelIndex topLeft = index;
        QModelIndex bottomRight = index;
        emit dataChanged(topLeft, bottomRight);
        return true;
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

void ChannelModel::addPlot()
{
    numberGraph++;
    graphNames.append("_");

    for(int i = 0; i < m_channels->size(); i++)
    {
        m_channels->at(i)->getPlotList()->resize(numberGraph);
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
            QVector<bool> * plotList = m_channels->at(i)->getPlotList();
            if(plotList->size() >= numberGraph)
            {
                plotList->removeAt(number);
            }//if size != numberGraph it is mean that there is error in var list
            else
            {
                plotList->resize(0);
                plotList->resize(numberGraph - 1);
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
