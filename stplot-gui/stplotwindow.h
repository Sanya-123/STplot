#ifndef STPLOTWINDOW_H
#define STPLOTWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include "DockManager.h"
extern "C" {
#include "varcommon.h"
}
#include "varloader.h"
#include "channels.h"
#include "viewmanager.h"
#include "readwritedevice.h"
#include "readloop.h"
#include <QThread>
#include <QTimer>
#include <QComboBox>
#include "readmanager.h"
#include "debugerwindow.h"
#include "settingswindow.h"
#include "QtAdvancedStylesheet.h"
#include "fileprogress.h"


#define MINIMUM_PLUGIN_READ_WRITE_DEVICE_HEADER_VERSION             0x00020000


QT_BEGIN_NAMESPACE
namespace Ui { class STPlotWindow; }
QT_END_NAMESPACE

class STPlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    STPlotWindow(DebugerWindow *debuger = nullptr, QWidget *parent = nullptr);
    ~STPlotWindow();
    void setDebuger(DebugerWindow *debuger);
    void loadSettingsFromConfig(QString &path);
    void loadVarsFromConfig(QString &path);

public slots:
    void startRead();
    void stopedRead();
    void setReadDevice(int);
    void loadSettings();
    void saveSettings();
    void saveSettingsAs();
    void newSettings();
    void openSettingsReader();
    void applySettingsSlot();
    void showSettingsWindows();

private:
    void applySettings(QSettings &settings);
    void writeSettings(QSettings &settings);
    void saveSettingsToFile(QString fileName);
    void initReadDevice();
    void closeEvent(QCloseEvent *event);
    void updateStyle();

private:
    Ui::STPlotWindow *ui;
    DebugerWindow *debuger;
//    ads::CDockManager* m_DockManager;
    Channels* channelsView;
    VarLoader* varloader;
    ViewManager* viewManager;
    ReadManager readManager;
    QString curentSettingsPath;
    QTimer redrawTimer;
    QComboBox readSelector;
    QToolBar *runToolBar;
    QAction *lastReadWidget;
    FileProgress fileProgress;

    //read device
    QList<ReadDeviceObject*> readDeviceList;
    QList<SaveDeviceObject*> saveDeviceList;

    struct ReadDeviceInstance{
        ReadDeviceObject *object;
        QString name;
        QDialog *configDialog;
    };
    QList<struct ReadDeviceInstance> listReadDeviceInstance;

    struct SaveDeviceInstance{
        SaveDeviceObject *object;
        QString name;
        QDialog *configDialog;
    };
    QList<struct SaveDeviceInstance> listSaveDeviceInstance;

    //styles
    acss::QtAdvancedStylesheet advancedStylesheet;

    //cettings
    SettingsWindow::SettingsMainWindow currentSettings;
    SettingsWindow *settingsWindow;

    //easys wayr to tes allow SettingsWindow to get prival warible amd get setting from it
    friend class SettingsWindow;
};





#endif // STPLOTWINDOW_H
