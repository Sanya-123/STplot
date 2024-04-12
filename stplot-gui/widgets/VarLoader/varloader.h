#ifndef VARLOADER_H
#define VARLOADER_H

#include <QWidget>
#include <QFileSystemWatcher>
#include "varmodel.h"
#include "varfilter.h"

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
#ifndef Q_OS_WINDOWS
    bool isElfLoaded();
#endif

public slots:
#ifndef Q_OS_WINDOWS
    void openElf();
    void updateElf(const QString &);
    void loadElf();
    void addVariables();
    void applyFilter(const QString &);
    void collapseTree();
    void expandTree();
#endif

signals:
    void variableAdded(varloc_node_t*);
    void variablesUpdated(varloc_node_t*);

private:
#ifndef Q_OS_WINDOWS
    void loadVariables(const QString &);
#endif
    Ui::VarLoader *ui;
#ifndef Q_OS_WINDOWS
    VarFilter *proxyModel;
    // QSortFilterProxyModel *proxyModel;
    VarModel *varModel;
#endif
    QFileSystemWatcher watcher;
};

#endif // VARLOADER_H
