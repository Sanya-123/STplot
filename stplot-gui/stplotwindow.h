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
#include "stlinkdevice.h"
#include "readloop.h"
#include <QThread>
#include <QTimer>
#include <QComboBox>
#include "readmanager.h"
#include "simplereader.h"
#include "shnet_socket_device.h"
#include "debugerwindow.h"
#include "settingswindow.h"
#include "QtAdvancedStylesheet.h"


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

public slots:
    void startRead();
    void stopedRead();
    void setReadDevice(int);
    // void read();
    void loadSettings();
    void saveSettings();
    void saveSettingsAs();
    void openSettingsReader();
    void applySettingsSlot();
    void showSettingsWindows();
//    void connect();

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
    // QVector<ReadDeviceObject*> readDeviceces;
    QVector<SaveDeviceObject*> saveDeviceces;
    ReadManager readManager;
    QString curentSettingsPath;
    QTimer redrawTimer;
    QComboBox readSelector;
    QToolBar *runToolBar;
    QAction *lastReadWidget;
    STlinkDevice stlinkDevice;
    SHnetUDPDevice shnetDevice;
    STMstudioFileDevice stmStudioSaveDevicec;

    struct ReadDeviceInstance{
        ReadDeviceObject *object;
        QString name;
        QWidget *configWidget;
    };
    QList<struct ReadDeviceInstance> listReadDeviceInstance;

    acss::QtAdvancedStylesheet advancedStylesheet;
    SettingsWindow::SettingsMainWindow currentSettings;
    SettingsWindow *settingsWindow;

    //easys wayr to tes allow SettingsWindow to get prival warible amd get setting from it
    friend class SettingsWindow;
};





#endif // STPLOTWINDOW_H
