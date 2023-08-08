#ifndef VARLOADER_H
#define VARLOADER_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include "varmodel.h"
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
    void load_variables(const QString &);

private:
    Ui::VarLoader *ui;
    QSortFilterProxyModel *proxyModel;
    VarModel *varModel;

public slots:
    void open_elf();
    void add_variables();
    void apply_filter(const QString &);
    void collapse_tree();
    void expand_tree();

signals:
    void variable_added(varloc_node_t*);
};

#endif // VARLOADER_H
