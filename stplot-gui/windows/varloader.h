#ifndef VARLOADER_H
#define VARLOADER_H

#include <QWidget>
#include <QSortFilterProxyModel>
extern "C" {
#include "varloc.h"
}
namespace Ui {
class VarLoader;
}

class VarLoader : public QWidget
{
    Q_OBJECT

public:
    explicit VarLoader(QWidget *parent = nullptr);
    ~VarLoader();

private:
    Ui::VarLoader *ui;
    QSortFilterProxyModel *proxyModel;
    varloc_node_t* tree_root;

public slots:
    void open_elf();
    void apply_filter(const QString &);
    void collapse_tree();
    void expand_tree();
};

#endif // VARLOADER_H
