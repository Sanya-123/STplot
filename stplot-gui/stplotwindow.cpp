#include "stplotwindow.h"
#include "./ui_stplotwindow.h"

#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QTreeView>
#include <QFile>
#include <QSettings>
#include <QDebug>

//#include <iostream>
//#include <format>
#include "varmodel.h"
#include "varloader.h"
//#include <elfio/elfio_dump.hpp>
#include "channels.h"
extern "C" {
#include "stlink.h"
#include "usb.h"
// #include <chipid.h>
// #include <helper.h>
// #include "logging,h"
}


using namespace ads;

STPlotWindow::STPlotWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::STPlotWindow)
{
    ui->setupUi(this);
//    proxyModel = new QSortFilterProxyModel(this);

//    // Create the dock manager. Because the parent parameter is a QMainWindow
//    // the dock manager registers itself as the central widget.
//    QVBoxLayout* Layout = new QVBoxLayout(ui->dockContainer);
//    Layout->setContentsMargins(QMargins(0, 0, 0, 0));
//    m_DockManager = new ads::CDockManager(ui->dockContainer);
//    Layout->addWidget(m_DockManager);

    ads::CDockWidget* dockWidgetChanaleWivw = new ads::CDockWidget("Channels");
    channelsView = new Channels(dockWidgetChanaleWivw);
    dockWidgetChanaleWivw->setWidget(channelsView);

    ads::CDockWidget* dockWidgetVarLoader = new ads::CDockWidget("VarLoader");
    varloader = new VarLoader(dockWidgetVarLoader);
    dockWidgetVarLoader->setWidget(varloader);

    ads::CDockWidget* dockWidgetViwManager = new ads::CDockWidget("ViewManager");
    viewManager = new ViewManager(dockWidgetViwManager);
    dockWidgetViwManager->setWidget(viewManager);

    // Add the toggleViewAction of the dock widget to the menu to give
    // the user the possibility to show the dock widget if it has been closed
    ui->menuView->addAction(dockWidgetChanaleWivw->toggleViewAction());
    ui->menuView->addAction(dockWidgetVarLoader->toggleViewAction());
    ui->menuView->addAction(dockWidgetViwManager->toggleViewAction());
    ui->menuView->addSeparator();

    ui->dockContainer->addDockWidget(ads::RightDockWidgetArea, dockWidgetChanaleWivw);
//    m_DockManager->addDockWidget(ads::LeftDockWidgetArea, DockWidget2);
    ui->dockContainer->addDockWidget(ads::NoDockWidgetArea, dockWidgetVarLoader);
    ui->dockContainer->addDockWidget(ads::NoDockWidgetArea, dockWidgetViwManager);

    QObject::connect(varloader, &VarLoader::variable_added, channelsView, &Channels::add_channel);
    QObject::connect(ui->menuRun, &QMenu::triggered, this, &STPlotWindow::startRead);

    viewManager->setDockContainer(ui->dockContainer);
    viewManager->setMenuView(ui->menuView);
    viewManager->setChanales(channelsView);

    //init devicec
//    readDeviceces.append(&stlinkDevice);
    readManager.setReadDevicece(&stlinkDevice);

    //restore settings
    QSettings settings("STdebuger", "STplotDebuger");
    restoreGeometry(settings.value("windows/geometry").toByteArray());
    restoreState(settings.value("windows/state").toByteArray());

    settings.beginGroup("channels");
    channelsView->restoreSettings(&settings);
    settings.endGroup();

    settings.beginGroup("varloader");
    varloader->restoreSettings(&settings);
    settings.endGroup();

    settings.beginGroup("viewmanager");
    viewManager->restoreSettings(&settings);
    settings.endGroup();

    ui->dockContainer->restoreState(settings.value("windows/docker/state").toByteArray());

}

STPlotWindow::~STPlotWindow()
{
    //save settings
    QSettings settings("STdebuger", "STplotDebuger");
    settings.setValue("windows/state", saveState());
    settings.setValue("windows/geometry", saveGeometry());
    settings.setValue("windows/docker/state", ui->dockContainer->saveState());
    //chanale
    settings.beginGroup("channels");
    channelsView->saveSettings(&settings);
    settings.endGroup();
    //varloader
    settings.beginGroup("varloader");
    varloader->saveSettings(&settings);
    settings.endGroup();
    //
    settings.beginGroup("viewmanager");
    viewManager->saveSettings(&settings);
    settings.endGroup();
    delete ui;
}

void STPlotWindow::read(){
    if (!simpleReader.isConnected()){
        if (simpleReader.connect() < 0){
            qInfo( "Error connecting stlink" );
        }
        simpleReader.loadChannels(channelsView->getListChanales());
    }
    else{
        if(simpleReader.readData() < 0){
            qInfo( "Error getting data" );
        }
    }

}

void STPlotWindow::startRead()
{

//    readManager.runReadLoop(channelsView->getListChanales());
//    simpleReader.readChannels(channelsView->getListChanales());
//    QVector<VarChannel *> *channels =  channelsView->getListChanales();
//    for (int i = 0; i < channels->size(); ++i) {
//        channels->at(i)->pushValueRaw(0x755);
//    }

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &STPlotWindow::read);
    timer->start(10);

}


//void STPlotWindow::closeEvent(QCloseEvent *event)
//{
//    QMainWindow::closeEvent(event);
//    if (m_DockManager)
//    {
//        m_DockManager->deleteLater();
//    }
//}

//void STPlotWindow::connect(){
//    stlink_t* sl = NULL;
//    sl = stlink_open_usb(UDEBUG, CONNECT_HOT_PLUG, NULL, 100000);
//    // if (sl == NULL){
//    //     return(-1);
//    // }
//}

