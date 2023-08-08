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
    explicit VarModel(QObject *parent = nullptr);
    ~VarModel();

    void            setModelRoot(varloc_node_t *root);
    varloc_node_t*  getModelRoot();
    bool            setData(const QModelIndex &index, const QVariant &value,
                            int role = Qt::EditRole) override;
    QVariant        data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags   flags(const QModelIndex &index) const override;
    QVariant        headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const override;
    QModelIndex     index(int row, int column,
                            const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex     parent(const QModelIndex &index) const override;
    int             rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int             columnCount(const QModelIndex &parent = QModelIndex()) const override;
    void            deselect_all();
    QMap<varloc_node_t*, bool> & get_selected_nodes();
//    QString*        get_node_uniqe_name(varloc_node_t *node);
private:
//    void setupModelData(const QStringList &lines, varloc_node_t *parent);
    QMap<varloc_node_t*, bool> selection_map;
    varloc_node_t *rootItem;
    void select_node(varloc_node_t* node);
    void deselect_node(varloc_node_t* node);
    void apply_for_each_child(varloc_node_t* root, void (VarModel::*func)(varloc_node_t*));

};

#endif // VARMODEL_H
