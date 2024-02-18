#ifndef VARFILTER_H
#define VARFILTER_H

#include <QSortFilterProxyModel>

class VarFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit VarFilter(QObject *parent = 0);

signals:

public slots:

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool filterAcceptsRowItself(int source_row, const QModelIndex &source_parent) const;
    bool hasAcceptedChildren(int source_row, const QModelIndex &source_parent) const;

};

#endif // VARFILTER_H
