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
#include <QMessageBox>

#include "varmodel.h"
#include "varloader.h"
#include "channels.h"
#include "stplotpluginloader.h"


using namespace ads;

STPlotWindow::STPlotWindow(DebugerWindow *debuger, QWidget *parent)
    : QMainWindow(parent), debuger(debuger)
    , ui(new Ui::STPlotWindow)
{
    ui->setupUi(this);

    ui->menuView->addAction("Debuger")->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
    setDebuger(debuger);

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

    //init list devicec
    initReadDevice();

    runToolBar = addToolBar("Run");
    runToolBar->addAction(ui->actionStart);
    runToolBar->addAction(ui->actionStop);
    runToolBar->setObjectName("runToolBar");
    runToolBar->addWidget(&readSelector);
    runToolBar->addAction(qApp->style()->standardIcon(QStyle::SP_FileDialogDetailedView), "Config (ctrl + O)", this, SLOT(openSettingsReader()))->
            setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    ui->menuView->addAction(runToolBar->toggleViewAction());

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
    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(newSettings()));
    connect(&readSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(setReadDevice(int)));
    connect(channelsView, SIGNAL(requestWriteData(uint64_t,varloc_location_t)), &readManager, SLOT(requestWriteData(uint64_t,varloc_location_t)));
    connect(ui->actionMainConfig, SIGNAL(triggered(bool)), this, SLOT(showSettingsWindows()));


    viewManager->setDockContainer(ui->dockContainer);
    viewManager->setMenuView(ui->menuView);
    viewManager->setChanales(channelsView);

    connect(&redrawTimer, &QTimer::timeout, viewManager, &ViewManager::updateAllViews);
    connect(channelsView, SIGNAL(requestReplot()), viewManager, SLOT(updateAllViews()));

    //init default read device
    if(listReadDeviceInstance.size() != 0)
        readManager.setReadDevicece(listReadDeviceInstance[0].object);
    foreach (SaveDeviceObject* dev, saveDeviceList) {
        readManager.addSaveDevice(dev);
    }

    //init style
    advancedStylesheet.setStylesDirPath(QDir::currentPath() + "/styles/");
    advancedStylesheet.setOutputDirPath(QDir::currentPath() + "/stylesoutput");
    advancedStylesheet.setCurrentStyle("qt_material");
    advancedStylesheet.setDefaultTheme();
    currentSettings.theme = advancedStylesheet.currentTheme();
//    updateStyle();

    //init settengs
    settingsWindow = new SettingsWindow(this, this);

    //restore settings
    QSettings settings("STdebuger", "STplotDebuger");
    applySettings(settings);
}

STPlotWindow::~STPlotWindow()
{
    //save settings
    QSettings settings("STdebuger", "STplotDebuger");
    writeSettings(settings);
    delete ui;
}

void STPlotWindow::setDebuger(DebugerWindow *debuger)
{
    this->debuger = debuger;
    if(debuger != nullptr)
    {
        connect(ui->menuView->actions()[0], SIGNAL(triggered(bool)), debuger, SLOT(show()));
        connect(ui->menuView->actions()[0], SIGNAL(triggered(bool)), debuger, SLOT(raise()));
    }
}

void STPlotWindow::setReadDevice(int readDevice){
    if(readDevice < listReadDeviceInstance.size())
    {
        readManager.setReadDevicece(listReadDeviceInstance[readDevice].object);
//        qDebug() << "Read device " << listReadDeviceInstance[readDevice].name;
    }
}

void STPlotWindow::startRead()
{
    if(currentSettings.autoclenaOnStart)
        channelsView->clearAllChanales();
    readSelector.setDisabled(true);
    ui->actionStart->setEnabled(false);
    ui->actionStop->setEnabled(true);
    redrawTimer.start(100);
    readManager.runReadLoop(channelsView->getListChanales(), channelsView->getListMathChanales());
}

void STPlotWindow::stopedRead()
{
    readSelector.setDisabled(false);
    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(false);
    redrawTimer.stop();
    viewManager->updateAllViews();
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
            varloader->loadTree();
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

void STPlotWindow::newSettings()
{
    if(QMessageBox::question(this, "New config", "All windows, chanales and plots will be clean; Are you ready to create new config?") == QMessageBox::Yes)
    {
        QFile emptyConfig("tmp.conf");
        emptyConfig.open(QIODevice::WriteOnly);
        emptyConfig.write(QByteArray());
        emptyConfig.close();
        QSettings settings(emptyConfig.fileName(), QSettings::IniFormat);
        applySettings(settings);
        emptyConfig.remove();
    }
}

void STPlotWindow::openSettingsReader()
{
    int index = readSelector.currentIndex();
    if(index < listReadDeviceInstance.size())
    {
        QDialog *configDialog = listReadDeviceInstance[index].configDialog;
        if(configDialog != nullptr)
        {
            configDialog->show();
            configDialog->raise();
            configDialog->exec();
        }
    }
}

void STPlotWindow::applySettingsSlot()
{
    settingsWindow->getSettings(currentSettings);
    updateStyle();
}

void STPlotWindow::showSettingsWindows()
{
    settingsWindow->setSettings(currentSettings);
    settingsWindow->show();
    settingsWindow->exec();
}

void STPlotWindow::applySettings(QSettings &settings)
{
    //NOTE do not forget clean some stuff like gruphs that should me implementer in restore settings

    restoreGeometry(settings.value("windows/geometry").toByteArray());
    restoreState(settings.value("windows/state").toByteArray());
    currentSettings.theme = settings.value("windows/theme", "none").toString();
    currentSettings.autoclenaOnStart = settings.value("readconfig/autocleanchanales").toBool();
    readSelector.setCurrentIndex(settings.value("device/selectedreaddev").toInt());

    settings.beginGroup("varloader");
    varloader->restoreSettings(&settings);
    settings.endGroup();

    settings.beginGroup("viewmanager");
    viewManager->restoreSettings(&settings);
    settings.endGroup();

    settings.beginGroup("channels");
    channelsView->restoreSettings(&settings);
    settings.endGroup();

    //restore RW plugins settings
    settings.beginGroup("readdevileplugins");
    for(ReadDeviceInstance dev : listReadDeviceInstance)
    {
        settings.beginGroup(dev.name);
        dev.object->restoreSettings(&settings);
        settings.endGroup();
    }
    settings.endGroup();
    settings.beginGroup("savedevileplugins");
    for(SaveDeviceInstance dev : listSaveDeviceInstance)
    {
        settings.beginGroup(dev.name);
        dev.object->restoreSettings(&settings);
        settings.endGroup();
    }
    settings.endGroup();

    if(debuger != nullptr)
    {
        settings.beginGroup("debuger");
        debuger->restoreSettings(&settings);
        settings.endGroup();
    }

    ui->dockContainer->restoreState(settings.value("windows/docker/state").toByteArray());

    updateStyle();
}

void STPlotWindow::writeSettings(QSettings &settings)
{
    settings.setValue("windows/state", saveState());
    settings.setValue("windows/geometry", saveGeometry());
    settings.setValue("windows/docker/state", ui->dockContainer->saveState());
    settings.setValue("windows/theme", currentSettings.theme);
    settings.setValue("readconfig/autocleanchanales", currentSettings.autoclenaOnStart);
    settings.setValue("device/selectedreaddev", readSelector.currentIndex());
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

    //save RW plugins settings
    settings.beginGroup("readdevileplugins");
    for(ReadDeviceInstance dev : listReadDeviceInstance)
    {
        settings.beginGroup(dev.name);
        dev.object->saveSettings(&settings);
        settings.endGroup();
    }
    settings.endGroup();
    settings.beginGroup("savedevileplugins");
    for(SaveDeviceInstance dev : listSaveDeviceInstance)
    {
        settings.beginGroup(dev.name);
        dev.object->saveSettings(&settings);
        settings.endGroup();
    }
    settings.endGroup();

    if(debuger != nullptr)
    {
        settings.beginGroup("debuger");
        debuger->saveSettings(&settings);
        settings.endGroup();
    }
}

void STPlotWindow::saveSettingsToFile(QString fileName)
{
    QSettings settings(fileName, QSettings::IniFormat);
    if(settings.isWritable())
    {
        writeSettings(settings);
    }
}

void STPlotWindow::initReadDevice()
{
    //load plugin device
    QList<ReadDeviceInterfacePlugin*> pluginsReadDevice = loadPlugin<ReadDeviceInterfacePlugin>(MINIMUM_PLUGIN_READ_WRITE_DEVICE_HEADER_VERSION, READ_WRITE_DEVICE_INTERFACE_HEADER_VERSION);
    QList<SaveDeviceInterfacePlugin*> pluginsWriteDevice = loadPlugin<SaveDeviceInterfacePlugin>(MINIMUM_PLUGIN_READ_WRITE_DEVICE_HEADER_VERSION, READ_WRITE_DEVICE_INTERFACE_HEADER_VERSION);

    struct ReadDeviceInstance devRead;
    struct SaveDeviceInstance devSave;

    for(int i = 0; i < pluginsReadDevice.size(); i++)
    {
        ReadDeviceObject *readDevie = pluginsReadDevice[i]->createReadDeviceObject();
        readDeviceList.append(readDevie);
        devRead.object = readDevie;
        devRead.name = pluginsReadDevice[i]->getName();
        devRead.configDialog = readDevie->getReadDevConfigDialog();
        listReadDeviceInstance.append(devRead);
    }

    for(int i = 0; i < pluginsWriteDevice.size(); i++)
    {
        SaveDeviceObject *saveDevice = pluginsWriteDevice[i]->createWriteDeviceObject();
//        readDeviceList.append(saveDevice);
        devSave.object = saveDevice;
        devSave.name = pluginsWriteDevice[i]->getName();
        devSave.configDialog = saveDevice->getSaveDevConfigDialog();
        listSaveDeviceInstance.append(devSave);
        saveDeviceList.append(saveDevice);
    }

    for(int i = 0; i < listReadDeviceInstance.size(); i++)
    {
        readSelector.addItem(listReadDeviceInstance[i].name);
        if(listReadDeviceInstance[i].configDialog != nullptr)
        {
            ui->menuconfing->addAction(listReadDeviceInstance[i].name, [=] () {
                listReadDeviceInstance[i].configDialog->show();
                listReadDeviceInstance[i].configDialog->raise();
                listReadDeviceInstance[i].configDialog->exec();
            });
        }

    }
}

void STPlotWindow::closeEvent(QCloseEvent *event)
{
    if(debuger != nullptr)
        debuger->close();
    readManager.stopRead();
    QMainWindow::closeEvent(event);
}

void STPlotWindow::updateStyle()
{
    if(currentSettings.theme == "none")
    {
        qApp->setPalette(this->style()->standardPalette());
        qApp->setStyleSheet("");
    }
    else
    {
        advancedStylesheet.setCurrentTheme(currentSettings.theme);
        advancedStylesheet.updateStylesheet();
        qApp->setStyleSheet(advancedStylesheet.styleSheet());
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


