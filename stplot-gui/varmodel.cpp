#include "varmodel.h"
#include <QStringList>

VarModel::VarModel(varloc_node_t *root, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new_var_node();
    rootItem->name = "variables";
    rootItem->child = root;
}

VarModel::~VarModel()
{
    varloc_delete_tree(rootItem);
}


int VarModel::columnCount(const QModelIndex &parent) const
{
    return 5;
//    varloc_node_t *node = NULL;
//    int count = 0;
//    if (parent.isValid()){
//        node = static_cast<varloc_node_t*>(parent.internalPointer());
//    }
//    else{
//        node = rootItem;
//    }

//    while(node->next){
//        count++;
//        node = node->next;
//    }
//    return count;
}


QVariant VarModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    varloc_node_t *item = static_cast<varloc_node_t*>(index.internalPointer());
    if (index.column() == 0){
        if ((item->name == NULL)
            //        || (item->name[0] = '\0')
            ){
            return QVariant("none");
        }
        else{
            return QVariant(QString::fromStdString(item->name));
        }
    }
    else if (index.column() == 1){
        if ((item->ctype_name[0] == 0)
            //        || (item->name[0] = '\0')
        ){
            return QVariant("none");
        }
        else{
            return QVariant(QString::fromStdString(item->ctype_name));
        }
    }
    else if (index.column() == 2){
        return QVariant(var_node_get_address(item));
    }
    else if (index.column() == 3){
        return QVariant(item->address.size_bits);
    }
    else if (index.column() == 4){
        return QVariant(var_node_get_type_name(item));
    }
    return QVariant();

}


Qt::ItemFlags VarModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}


QVariant VarModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
        if (section == 0){
            return QVariant("Name");
        }
        if (section == 1){
            return QVariant("CType");
        }
        if (section == 2){
            return QVariant("Address");
        }
        if (section == 3){
            return QVariant("Size");
        }
        if (section == 4){
            return QVariant("Type");
        }
    }
    return QVariant();
}


QModelIndex VarModel::index(int row, int column, const QModelIndex &parent) const
{
//    if (!hasIndex(row, column, parent))
//        return QModelIndex();
//    if(column == 0){
//        return QModelIndex();
//    }
    varloc_node_t *childNode = NULL;
    varloc_node_t *parentNode = NULL;

    if (!parent.isValid())
        parentNode = rootItem;
    else
        parentNode = static_cast<varloc_node_t*>(parent.internalPointer());

    childNode = var_node_get_child_at_index(parentNode, row);
    if (childNode == NULL){
        return QModelIndex();
    }

    return createIndex(row, column, childNode);
}


QModelIndex VarModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    varloc_node_t *child = static_cast<varloc_node_t*>(index.internalPointer());
    varloc_node_t *parent = NULL;

    if (child == rootItem){
        return QModelIndex();
    }

    parent = var_node_get_parent(child);
    if (parent == NULL){
        return QModelIndex();
    }

    return createIndex(0, 0, parent);
}


int VarModel::rowCount(const QModelIndex &parent) const
{
//    varloc_node_t *parentItem;
//    if (parent.column() > 0)
//        return 0;

    varloc_node_t *childNode = NULL;
    varloc_node_t *parentNode = NULL;
    if (!parent.isValid())
        parentNode = rootItem;
    else
        parentNode = static_cast<varloc_node_t*>(parent.internalPointer());

    int row_n = 0;
    childNode = parentNode->child;
    if (childNode == NULL){
        return 0;
    }
    while (childNode->next != NULL){
        childNode = childNode->next;
        row_n++;
    }
    return row_n + 1;
}


