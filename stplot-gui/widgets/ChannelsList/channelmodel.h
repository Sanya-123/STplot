#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "varchannel.h"
class ChannelModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ChannelModel(QVector<VarChannel*> *channels, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
//    Qt::ItemFlags flags(const QModelIndex &index) const override;
private:
    QVector<VarChannel*> *m_channels;

};

#endif // CHANNELMODEL_H
