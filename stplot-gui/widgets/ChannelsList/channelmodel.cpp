#include "channelmodel.h"

ChannelModel::ChannelModel( QVector<VarChannel*> *channels, QObject *parent)
    : QAbstractTableModel{parent}
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
    return 2;
}

QVariant ChannelModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.column() == 0){
        QString* name = m_channels->at(index.row())->name();
        return QVariant(*name);
    }
    else if(index.column() == 1){
        return QVariant(m_channels->at(index.row())->value());
    }
    return QVariant();
}
