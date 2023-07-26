#ifndef VARMODEL_H
#define VARMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
extern "C" {
#include "varloc.h"
}


class VarModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit VarModel(varloc_node_t *root, QObject *parent = nullptr);
    ~VarModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
//    void setupModelData(const QStringList &lines, varloc_node_t *parent);

    varloc_node_t *rootItem;
};

#endif // VARMODEL_H
