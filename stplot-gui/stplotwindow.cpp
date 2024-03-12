#include "stplotwindow.h"
#include "./ui_stplotwindow.h"

#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QTreeView>
#include <QFile>
#include <QSettings>
#include <QDebug>
#include <QToolBar>
#include <QFileDialog>


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
    qApp;
    ui->setupUi(this);
//    proxyModel = new QSortFilterProxyModel(this);

//    // Create the dock manager. Because the parent parameter is a QMainWindow
//    // the dock manager registers itself as the central widget.
//    QVBoxLayout* Layout = new QVBoxLayout(ui->dockContainer);
//    Layout->setContentsMargins(QMargins(0, 0, 0, 0));
//    m_DockManager = new ads::CDockManager(ui->dockContainer);
//    Layout->addWidget(m_DockManager);

    ui->dockContainer->setConfigFlag(ads::CDockManager::FocusHighlighting, true);


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

    //toolbar and menu
    ui->actionStart->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPlay));
    ui->actionStop->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPause));
    ui->actionStop->setDisabled(true);

    readSelector.insertItem(0,"STLink", QVariant());
    readSelector.insertItem(1,"UDP", QVariant());

    runToolBar = addToolBar("Run");
    runToolBar->addAction(ui->actionStart);
    runToolBar->addAction(ui->actionStop);
    runToolBar->setObjectName("runToolBar");
    runToolBar->addWidget(&readSelector);
    ui->menuView->addAction(runToolBar->toggleViewAction());
    lastReadWidget  = runToolBar->addWidget(stlinkDevice.getReadDevConfigWidget());

    ui->menuView->addSeparator();

    ui->dockContainer->addDockWidget(ads::RightDockWidgetArea, dockWidgetChanaleWivw);
//    m_DockManager->addDockWidget(ads::LeftDockWidgetArea, DockWidget2);
    ui->dockContainer->addDockWidget(ads::NoDockWidgetArea, dockWidgetVarLoader);
    ui->dockContainer->addDockWidget(ads::NoDockWidgetArea, dockWidgetViwManager);

    connect(varloader, &VarLoader::variableAdded, channelsView, &Channels::add_channel);
    connect(varloader, SIGNAL(variablesUpdated(varloc_node_t*)), channelsView, SLOT(reloadChannels(varloc_node_t*)));
    connect(ui->actionStart, &QAction::triggered, this, &STPlotWindow::startRead);
    connect(&readManager, SIGNAL(stopingRead()), this, SLOT(stopedRead()));
    connect(ui->actionStop, SIGNAL(triggered(bool)), &readManager, SLOT(stopRead()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(loadSettings()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(saveSettings()));
    connect(ui->actionSave_as, SIGNAL(triggered(bool)), this, SLOT(saveSettingsAs()));
    connect(&readSelector, SIGNAL(activated(int)), this, SLOT(setReadDevice(int)));


    viewManager->setDockContainer(ui->dockContainer);
    viewManager->setMenuView(ui->menuView);
    viewManager->setChanales(channelsView);

    connect(&redrawTimer, &QTimer::timeout, viewManager, &ViewManager::updateAllViews);

    //init default read device
    readManager.setReadDevicece(&stlinkDevice);

    //restore settings
    // QSettings settings("STdebuger", "STplotDebuger");
    // applySettings(settings);
}

STPlotWindow::~STPlotWindow()
{
    //save settings
    // QSettings settings("STdebuger", "STplotDebuger");
    // writeSettings(settings);
    delete ui;
}

// void STPlotWindow::read(){
//     if (!simpleReader.isConnected()){
//         if (simpleReader.connect() < 0){
//             qInfo( "Error connecting stlink" );
//         }
//         simpleReader.loadChannels(channelsView->getListChanales());
//     }
//     else{
//         if(simpleReader.readData() < 0){
//             qInfo( "Error getting data" );
//         }
//     }

// }
void STPlotWindow::setReadDevice(int readDevice){
    switch(readDevice){
    case 0:
        readManager.setReadDevicece(&stlinkDevice);
        runToolBar->removeAction(lastReadWidget);
        lastReadWidget = runToolBar->addWidget(stlinkDevice.getReadDevConfigWidget());
        qDebug() << "Read device STLINK";
        break;
    case 1:
        readManager.setReadDevicece(&shnetDevice);
        runToolBar->removeAction(lastReadWidget);
        lastReadWidget = runToolBar->addWidget(shnetDevice.getReadDevConfigWidget());
        qDebug() << "Read device UDP";
        break;
    }
}

void STPlotWindow::startRead()
{
    readSelector.setDisabled(true);
    ui->actionStart->setEnabled(false);
    ui->actionStop->setEnabled(true);
    redrawTimer.start(100);
    readManager.runReadLoop(channelsView->getListChanales());
}

void STPlotWindow::stopedRead()
{
    readSelector.setDisabled(false);
    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(false);
    redrawTimer.stop();
}

void STPlotWindow::loadSettings()
{
    QString newSettings = QFileDialog::getOpenFileName(this, "Load settings", curentSettingsPath, "Configs(*.conf);;All(*)");
    if(!newSettings.isEmpty())
    {
        curentSettingsPath = newSettings;
        QSettings settings(curentSettingsPath, QSettings::IniFormat);
        if(settings.status() == QSettings::NoError)
        {
            applySettings(settings);
            // Try to load elf file and update channel addresses
            varloader->loadElf();
        }
    }
}

void STPlotWindow::saveSettings()
{
    if(curentSettingsPath.isEmpty())
        return saveSettingsAs();

    saveSettingsToFile(curentSettingsPath);
}

void STPlotWindow::saveSettingsAs()
{
    QString newSettings = QFileDialog::getSaveFileName(this, "Load settings", curentSettingsPath, "Configs(*.conf);;All(*)");
    if(!newSettings.isEmpty())
    {
        //check syfix
        if(((QStringList)newSettings.split("/")).last().indexOf('.') == -1)
            newSettings.append(".conf");

        curentSettingsPath = newSettings;
        saveSettingsToFile(curentSettingsPath);
    }
}

void STPlotWindow::applySettings(QSettings &settings)
{
    //NOTE do not forget clean some stuff like gruphs that should me implementer in restore settings

    restoreGeometry(settings.value("windows/geometry").toByteArray());
    restoreState(settings.value("windows/state").toByteArray());

    settings.beginGroup("varloader");
    varloader->restoreSettings(&settings);
    settings.endGroup();

    settings.beginGroup("viewmanager");
    viewManager->restoreSettings(&settings);
    settings.endGroup();

    settings.beginGroup("channels");
    channelsView->restoreSettings(&settings);
    settings.endGroup();

    ui->dockContainer->restoreState(settings.value("windows/docker/state").toByteArray());
}

void STPlotWindow::writeSettings(QSettings &settings)
{
    settings.setValue("windows/state", saveState());
    settings.setValue("windows/geometry", saveGeometry());
    settings.setValue("windows/docker/state", ui->dockContainer->saveState());
    //chanale
    settings.beginGroup("channels");
    channelsView->saveSettings(&settings);
    settings.endGroup();
    //
    settings.beginGroup("viewmanager");
    viewManager->saveSettings(&settings);
    settings.endGroup();
    //varloader
    settings.beginGroup("varloader");
    varloader->saveSettings(&settings);
    settings.endGroup();
}

void STPlotWindow::saveSettingsToFile(QString fileName)
{
    QSettings settings(fileName, QSettings::IniFormat);
    if(settings.isWritable())
    {
        writeSettings(settings);
    }
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

