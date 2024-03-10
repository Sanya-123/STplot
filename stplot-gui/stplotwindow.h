#ifndef STPLOTWINDOW_H
#define STPLOTWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include "DockManager.h"
extern "C" {
#include "varloc.h"
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


QT_BEGIN_NAMESPACE
namespace Ui { class STPlotWindow; }
QT_END_NAMESPACE

class STPlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    STPlotWindow(QWidget *parent = nullptr);
    ~STPlotWindow();

public slots:
    void startRead();
    void stopedRead();
    void setReadDevice(int);
    // void read();
    void loadSettings();
    void saveSettings();
    void saveSettingsAs();
//    void connect();

private:
    void applySettings(QSettings &settings);
    void writeSettings(QSettings &settings);
    void saveSettingsToFile(QString fileName);

private:
    Ui::STPlotWindow *ui;
//    ads::CDockManager* m_DockManager;
    Channels* channelsView;
    VarLoader* varloader;
    ViewManager* viewManager;
    // QVector<ReadDeviceObject*> readDeviceces;
    QVector<SaveDeviceObject*> saveDeviceces;
    STlinkDevice stlinkDevice;
    ReadManager readManager;
    SHnetUDPDevice shnetDevice;
    QString curentSettingsPath;
    QTimer redrawTimer;
    QComboBox readSelector;
    QToolBar *runToolBar;
    QAction *lastReadWidget;
};





#endif // STPLOTWINDOW_H
