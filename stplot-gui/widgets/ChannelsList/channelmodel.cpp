#include "channelmodel.h"
#include <QDebug>

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
        //TODO
        else if(index.column() == 1){
            return QVariant(/*m_channels->at(index.row())->value()*/0);
        }
        else
            return QVariant("enable");
    }
    else if (role == Qt::CheckStateRole)
    {
        //set column graph as checke
        if(index.column() >= 2)
            return Qt::Checked;
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
        else if((section - 2) <  numberGraph)
        {
            return QVariant(graphNames[section - 2]);
        }
    }
    return QVariant();
}

bool ChannelModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && index.isValid())
    {//TODO
        if(index.column() >= 2)
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
    if (!index.isValid() || index.column() >= (numberGraph + 2))
        return QAbstractTableModel::flags(index);

    Qt::ItemFlags flags = QAbstractTableModel::flags(index);

    if (index.column() >= 2)
        return /*flags | */Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return flags;
}

void ChannelModel::addPlot()
{
    numberGraph++;
    graphNames.append("_");
    emit updateViewport();
    emit layoutChanged();
}

void ChannelModel::deletePlot(int number)
{
    if(number < numberGraph)
    {
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
