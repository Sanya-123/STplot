#ifndef VARLOADER_H
#define VARLOADER_H

#include <QWidget>
#include <QFileSystemWatcher>
#include "varmodel.h"
#include "varfilter.h"
#include "varreaderinterface.h"

extern "C" {
#include "varcommon.h"
}
#include <QSettings>

namespace Ui {
class VarLoader;
}

#define MINIMUM_PLUGIN_READER_HEADER_VERSION          0x00000000

class VarLoader : public QWidget
{
    Q_OBJECT

public:
    explicit VarLoader(QWidget *parent = nullptr);
    ~VarLoader();
    void saveSettings(QSettings *settings);
    void restoreSettings(QSettings *settings);
    bool isElfLoaded();

public slots:
    void openTree();
    void updateTree(const QString &);
    void loadTree();
    void saveTree();
    void addVariables();
    void applyFilter(const QString &);
    void collapseTree();
    void expandTree();

signals:
    void variableAdded(varloc_node_t*);
    void variablesUpdated(varloc_node_t*);

private:
    void loadVariables(const QString &);
    Ui::VarLoader *ui;
    VarFilter *proxyModel;
    // QSortFilterProxyModel *proxyModel;
    VarModel *varModel;
    QFileSystemWatcher watcher;
    QList<VarReadInterfacePlugin*> pluginsReader;
    QString allowReadFiles;
    QString allowWriteFiles;
    QMap<QString, VarReadInterfacePlugin*> mapPluginFile;
};

#endif // VARLOADER_H
