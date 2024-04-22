#include "varmodel.h"
#include <QStringList>

#define DEBUG_VIEW 0

VarModel::VarModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new_var_node();
    rootItem->child = NULL;
}

VarModel::~VarModel()
{
}

void VarModel::setModelRoot(varloc_node_t *root){
    rootItem->child = root;
    selection_map.clear();
}

varloc_node_t* VarModel::getModelRoot(){
    return rootItem->child;
}

int VarModel::columnCount(const QModelIndex &parent) const
{
#if DEBUG_VIEW == 1
    return 8;
#else
    return 5;
#endif
}

void VarModel::select_node(varloc_node_t* node)
{
    this->selection_map[node] = true;
}

void VarModel::deselect_node(varloc_node_t* node)
{
    this->selection_map[node] = false;
}

void VarModel::apply_for_each_child(varloc_node_t* root,
                                    void (VarModel::*func)(varloc_node_t*), bool first)
{
    if (root == NULL){
        return;
    }
    if (!first){
        (this->*(func))(root);
    }
     if (root->child != NULL){
         this->apply_for_each_child(root->child, func, false);
     }
     if (root->next != NULL && !first){
         this->apply_for_each_child(root->next, func, false);
     }
}

bool VarModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ((role != Qt::EditRole) && (role != Qt::CheckStateRole)){
        return false;
    }

    varloc_node_t *item = static_cast<varloc_node_t*>(index.internalPointer());

    if (index.column() == 0){
        if(value.toBool()){
            this->select_node(item);
            this->apply_for_each_child(item, &VarModel::select_node);
        }
        else{
            this->deselect_node(item);
            this->apply_for_each_child(item, &VarModel::deselect_node);
        }
       emit dataChanged(index, this->index(index.row()+1, index.column(), this->parent(index)), {Qt::DisplayRole, Qt::EditRole});
        return true;
    }
    return false;
}

QVariant VarModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    varloc_node_t *item = static_cast<varloc_node_t*>(index.internalPointer());

    if ( role == Qt::CheckStateRole && index.column() == 0 )
        return static_cast< int >( selection_map[item] ? Qt::Checked : Qt::Unchecked );

    if (role != Qt::DisplayRole)
        return QVariant();

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

        uint32_t address = var_node_get_address(item);
        QString res;
        if (address == 0xFFFFFFFF){
            res = QString("???");
        }
        else{
            res = QString("0x%1").arg(address, 8, 16, QLatin1Char( '0' ));
        }
        return QVariant(res);
    }
    else if (index.column() == 3){
        return QVariant(item->address.size_bits);
    }
    else if (index.column() == 4){
        return QVariant(var_node_get_type_name(item));
    }
#if DEBUG_VIEW == 1
    else if (index.column() == 5){
        return QVariant(item->address.offset_bits);
    }
    else if (index.column() == 6){
        return QVariant(var_node_get_load_location(item).address.offset_bits);
//        return QVariant(item->is_signed);
    }
    else if (index.column() == 7){
        return QString("0x%1").arg(var_node_get_load_location(item).address.base, 8, 16, QLatin1Char( '0' ));;
//        return QVariant(item->is_float);
    }
#endif
    return QVariant();

}


Qt::ItemFlags VarModel::flags(const QModelIndex &index) const
{

    if (!index.isValid())
        return Qt::NoItemFlags;
    varloc_node_t *item = static_cast<varloc_node_t*>(index.internalPointer());
    Qt::ItemFlags flags;
//    if (item->var_type == BASE){
        flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        if ( index.column() == 0 )
            flags |= Qt::ItemIsUserCheckable;
//    }
//    else{
//        flags = 0;
//    }
    return flags;
}


QVariant VarModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
        if (section == 0){
            return QVariant("Variable Name");
        }
        if (section == 1){
            return QVariant("CType");
        }
        if (section == 2){
            return QVariant("Address");
        }
        if (section == 3){
            return QVariant("Size bits");
        }
        if (section == 4){
            return QVariant("Type");
        }
#if DEBUG_VIEW == 1
        if (section == 5){
            return QVariant("Offset");
        }
        if (section == 6){
            return QVariant("Signed");
        }
        if (section == 7){
            return QVariant("Float");
        }
#endif
    }
    return QVariant();
}


QMap<varloc_node_t*, bool> & VarModel::get_selected_nodes(){
    QMap<varloc_node_t*, bool> &ref = selection_map;
    return ref;
}

void VarModel::deselect_all(){
    this->apply_for_each_child(rootItem, &VarModel::deselect_node);
    emit dataChanged(QModelIndex(), QModelIndex(), {Qt::DisplayRole, Qt::EditRole});
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
        return createIndex(row, column, parentNode);
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

    return createIndex(var_node_get_child_index(parent), 0, parent);
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


