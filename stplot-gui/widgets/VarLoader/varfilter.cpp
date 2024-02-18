#include "varfilter.h"
#include <QtDebug>
extern "C" {
#include "varloc.h"
}

VarFilter::VarFilter(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

bool VarFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    // varloc_node_t *source_node = static_cast<varloc_node_t*>(source_parent.internalPointer());
    if (filterAcceptsRowItself(source_row, source_parent)){
        return true;
    }
    else{
        return false;
    }

    //accept if any of the parents is accepted on it's own merits

    // if (hasAcceptedParents(source_row, source_parent)){

    // }
    QModelIndex parent = source_parent;
    QModelIndex parent_parent = source_parent.parent();
    while (parent.isValid()) {
        // source_node = static_cast<varloc_node_t*>(parent.internalPointer());
        if (filterAcceptsRowItself(parent.row(), parent.parent())){
            // qDebug() << "Accepted" << source_parent << source_row << parent.row() << parent.parent();
            return true;
        }
        parent = parent.parent();
    }

    //accept if any of the children is accepted on it's own merits
    if (hasAcceptedChildren(source_row, source_parent)) {
        return true;
    }

    return false;

}

bool VarFilter::filterAcceptsRowItself(int source_row, const QModelIndex &source_parent) const
{
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool VarFilter::hasAcceptedChildren(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex item = sourceModel()->index(source_row,0,source_parent);
    if (!item.isValid()) {
        //qDebug() << "item invalid" << source_parent << source_row;
        return false;
    }

    //check if there are children
    int childCount = item.model()->rowCount(item);
    if (childCount == 0)
        return false;

    for (int i = 0; i < childCount; ++i) {
        if (filterAcceptsRowItself(i, item))
            return true;
        //recursive call -> NOTICE that this is depth-first searching, you're probably better off with breadth first search...
        if (hasAcceptedChildren(i, item))
            return true;
    }

    return false;

}

bool VarFilter::hasAcceptedParents(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex item = sourceModel()->index(source_row,0,source_parent);
    if (!item.isValid()) {
        qDebug() << "item invalid" << source_parent << source_row;
        return false;
    }

    //check if there are parents
    QModelIndex parent = item.parent();
    if (!parent.isValid()) {
        qDebug() << "item invalid" << source_parent << source_row;
        return false;
    }

    return hasAcceptedParents(source_row, parent);
}
