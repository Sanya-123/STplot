#ifndef VARLOADER_H
#define VARLOADER_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include "varmodel.h"
extern "C" {
#include "varloc.h"
}
#include <QSettings>

namespace Ui {
class VarLoader;
}

class VarLoader : public QWidget
{
    Q_OBJECT

public:
    explicit VarLoader(QWidget *parent = nullptr);
    ~VarLoader();

    void saveSettings(QSettings *settings);
    void restoreSettings(QSettings *settings);

    void load_variables(const QString &);

public slots:
    void open_elf();
    void load_elf();
    void add_variables();
    void apply_filter(const QString &);
    void collapse_tree();
    void expand_tree();
    varloc_node_t* get_tree_root();

signals:
    void variable_added(varloc_node_t*);

private:
    Ui::VarLoader *ui;
    QSortFilterProxyModel *proxyModel;
    VarModel *varModel;
};

#endif // VARLOADER_H
