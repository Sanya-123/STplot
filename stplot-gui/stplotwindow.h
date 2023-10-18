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
#include "readmanager.h"
#include "simplereader.h"


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
    void read();
//    void connect();

private:
    Ui::STPlotWindow *ui;
//    ads::CDockManager* m_DockManager;
    Channels* channelsView;
    VarLoader* varloader;
    ViewManager* viewManager;
    QVector<ReadDeviceObject*> readDeviceces;
//    QVector<SaveDeviceObject*> saveDeviceces;
    STlinkDevice stlinkDevice;
//    ReadLoop readLoop;
//    QThread readLoopThread;
    ReadManager readManager;
    SimpleReader simpleReader;
};





#endif // STPLOTWINDOW_H
