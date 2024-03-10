#include "viewmanager.h"
#include "ui_viewmanager.h"
#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QMenu>
#include "settingsdialog.h"

ViewManager::ViewManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewManager), dockContainer(nullptr), menuView(nullptr), chanales(nullptr)
{
    ui->setupUi(this);
    loadPlugin();
    //add availeble graph to cobobocx
    for(int i = 0 ; i < pluginsPlot.size(); i++)
    {
        ui->comboBox_typeView->addItem(pluginsPlot[i]->getName());
    }
}

ViewManager::~ViewManager()
{
    delete ui;
}

void ViewManager::updateAllViews()
{
    QPair<PlotWidgetAbstract*, ads::CDockWidget*> view;
    foreach (view, listPlots){
        view.first->redraw();
    }
}

void ViewManager::saveSettings(QSettings *settings)
{
    QMap<QString,PlotWidgetInterfacePlugin*> plotMap;

    for(int i = 0; i < pluginsPlot.size(); i++)
        plotMap[pluginsPlot[i]->getName()] = pluginsPlot[i];

    settings->beginWriteArray("plots");
    for (int i = 0; i < listPlotParants.size(); i++) {
        settings->setArrayIndex(i);
        settings->setValue("pluginName", listPlotParants[i].second);
        settings->setValue("plotName", listPlotParants[i].first->windowTitle()/*objectName()*/);
        settings->setValue("plotSettings", listPlotParants[i].first->gedSettings()->getSettingsMap());
    }
    settings->endArray();

    //save deff settings
    settings->beginGroup("plotPluginSettings");
    for(int i = 0; i < pluginsPlot.size(); i++)
    {
        settings->setValue(pluginsPlot[i]->getName(), pluginsPlot[i]->gedDefauoldSettings()->getSettingsMap());
    }
    settings->endGroup();
}

void ViewManager::restoreSettings(QSettings *settings)
{
    //clean states
    ui->tableWidget_availebleWidgets->selectAll();
    on_pushButton_deleteViewes_clicked();

    QMap<QString,PlotWidgetInterfacePlugin*> plotMap;

    for(int i = 0; i < pluginsPlot.size(); i++)
        plotMap[pluginsPlot[i]->getName()] = pluginsPlot[i];

    int size = settings->beginReadArray("plots");
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QString pluginName = settings->value("pluginName").toString();
        QString plotName = settings->value("plotName").toString();
        QMap<QString, QVariant> gruphSettings = settings->value("plotSettings").toMap();

        if(plotMap.contains(pluginName))
        {
            addPlot(plotMap[pluginName], plotName)->gedSettings()->setSettings(gruphSettings);
        }
    }
    settings->endArray();

    //restore deff settings
    settings->beginGroup("plotPluginSettings");
    for(int i = 0; i < pluginsPlot.size(); i++)
    {
        QMap<QString, QVariant> gruphSettings = settings->value(pluginsPlot[i]->getName()).toMap();
        pluginsPlot[i]->gedDefauoldSettings()->setSettings(gruphSettings);
    }
    settings->endGroup();
}

void ViewManager::setDockContainer(ads::CDockManager *newDockContainer)
{
    dockContainer = newDockContainer;
}

void ViewManager::setMenuView(QMenu *newMenuView)
{
    menuView = newMenuView;
}

void ViewManager::setChanales(Channels *newChanales)
{
    chanales = newChanales;
    if(chanales != nullptr)
    {
        connect(chanales, SIGNAL(addingChanaleToPlot(VarChannel*,int,bool)), this, SLOT(addPotToPlot(VarChannel*,int,bool)));
    }
}

void ViewManager::loadPlugin()
{
    PlotWidgetInterfacePlugin *_interface;
    QDir pluginsDir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif

    if(!pluginsDir.cd("./plugins"))
        pluginsDir.cd("../plugins");

    qDebug() << "Plugin path:" << pluginsDir.absolutePath();

    const QStringList entries = pluginsDir.entryList(QDir::Files);//get list plugins files

    for (const QString &fileName : entries) {
        qDebug() << "Try load plugin:" << fileName;
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));//load file as plugin
        QObject *plugin = pluginLoader.instance();//get object plugin
        if (plugin) {
            _interface = qobject_cast<PlotWidgetInterfacePlugin *>(plugin);//check type plugin
            if (_interface)//!0 if type correct
            {
                VersionUnion _version;
                _version.version32 = _interface->getVersion();
                qDebug() << "OK plugin:" << _interface->getName() <<
                            QString::asprintf(";v%d.%d.%d", _version.versionS.major, _version.versionS.minor, _version.versionS.build);

                _version.versionS.build = 0;//do note abalize build version just major and minor

                //check version
                if(_version.version32 >= MINIMUM_PLUGIN_PLOT_HEADER_VERSION)
                {
                    if(_version.version32 <= PLOT_INTERFACE_HEADER_VERSION)//maximum avaleble version
                        pluginsPlot.append(_interface);
                    else
                    {
                        qDebug() << "App is too old for curent plugin please rebuild it";
                        pluginLoader.unload();
                    }
                }
                else
                {
                    qDebug() << "Plugin is too old please rebuild it";
                    pluginLoader.unload();
                }
            }
            else
            {
                qDebug() << "unload plugin:" << fileName;
                pluginLoader.unload();
            }
        }
        else{
            qDebug() << "Failed loading plugin";
        }
    }
}

void ViewManager::addPotToPlot(VarChannel *var, int numPlot, bool en)
{
    if(numPlot < 0 || numPlot >= listPlots.size())
        return;


    if(en)
        listPlots[numPlot].first->addPlot(var);
    else
        listPlots[numPlot].first->deletePlot(var);
}

PlotWidgetAbstract * ViewManager::addPlot(PlotWidgetInterfacePlugin *plotType, QString name)
{
    int numberRow = ui->tableWidget_availebleWidgets->rowCount();
    //create widgets for gui
    ads::CDockWidget* widgetDocker = new ads::CDockWidget(name);
    PlotWidgetAbstract *widgetPlot = plotType->createWidgetPlot(widgetDocker);
    widgetDocker->setWidget(widgetPlot);
    if(dockContainer != nullptr)
        dockContainer->addDockWidget(ads::RightDockWidgetArea, widgetDocker);

    if(menuView != nullptr)
        menuView->addAction(widgetDocker->toggleViewAction());
    //add to chanal modele
    if(chanales != nullptr)
        chanales->addPlot();

    listPlots.append(qMakePair(widgetPlot, widgetDocker));
    listPlotParants.append(qMakePair(widgetPlot, plotType->getName()));

    ui->tableWidget_availebleWidgets->setRowCount(numberRow + 1);
    ui->tableWidget_availebleWidgets->setItem(numberRow, 0, new QTableWidgetItem(name));

    return widgetPlot;
}

void ViewManager::on_tableWidget_availebleWidgets_cellChanged(int row, int column)
{
    if(row >= 0 && row < listPlots.size())
    {
        QString newName = ui->tableWidget_availebleWidgets->item(row, column)->text();
        listPlots[row].second->setWindowTitle(newName);
        listPlots[row].second->setObjectName(newName);
        listPlots[row].first->setWindowTitle(newName);
//        listPlots[row].second->setObjectName(newName);

        if(chanales != nullptr)
            chanales->setPlotName(row, newName);
    }
}

void ViewManager::on_pushButton_addView_clicked()
{
    int numberRow = ui->tableWidget_availebleWidgets->rowCount();
    if(ui->comboBox_typeView->currentIndex() >= 0 && ui->comboBox_typeView->currentIndex() < pluginsPlot.size())
    {
        //get selected plugin
        PlotWidgetInterfacePlugin* plotType = pluginsPlot[ui->comboBox_typeView->currentIndex()];
        addPlot(plotType, plotType->getName() + "_" + QString::number(numberRow));

//        //create widgets for gui
//        ads::CDockWidget* widgetDocker = new ads::CDockWidget("_");
//        PlotWidgetAbstract *widgetPlot = plotType->createWidgetPlot(widgetDocker);
//        widgetDocker->setWidget(widgetPlot);
//        if(dockContainer != nullptr)
//            dockContainer->addDockWidget(ads::RightDockWidgetArea, widgetDocker);

//        if(menuView != nullptr)
//            menuView->addAction(widgetDocker->toggleViewAction());
//        //add to chanal modele
//        if(chanales != nullptr)
//            chanales->addPlot();

//        listPlots.append(qMakePair(widgetPlot, widgetDocker));
//        listPlotParants.append(qMakePair(widgetPlot, plotType->getName()));

//        ui->tableWidget_availebleWidgets->setRowCount(numberRow + 1);
//        ui->tableWidget_availebleWidgets->setItem(numberRow, 0, new QTableWidgetItem(plotType->getName() + "_" +
//                                                      QString::number(numberRow)));
    }
}

void ViewManager::on_pushButton_deleteViewes_clicked()
{
    QList<QTableWidgetItem*> iteams = ui->tableWidget_availebleWidgets->selectedItems();

    for(int i = 0; i < iteams.size(); i++)
    {
        int numberElement = ui->tableWidget_availebleWidgets->row(iteams[i]);
        //remove from menu
        if(menuView != nullptr)
            menuView->removeAction(listPlots[numberElement].second->toggleViewAction());
        //remove from docker
        if(dockContainer != nullptr)
            dockContainer->removeDockWidget(listPlots[numberElement].second);
        //remove from  chanales model
        if(chanales != nullptr)
            chanales->deletePlot(numberElement);

        listPlots.removeAt(numberElement);
        listPlotParants.removeAt(numberElement);
        ui->tableWidget_availebleWidgets->removeRow(numberElement);
    }
}

void ViewManager::on_pushButton_open_def_setings_clicked()
{
    if(ui->comboBox_typeView->currentIndex() >= 0)
    {
        SettingsDialog *sett = new SettingsDialog(pluginsPlot[ui->comboBox_typeView->currentIndex()]->gedDefauoldSettings());
        sett->show();
        sett->exec();
    }
}

void ViewManager::on_pushButton_open_selected_plot_setings_clicked()
{
    QList<QTableWidgetItem*> iteams = ui->tableWidget_availebleWidgets->selectedItems();

    if(iteams.size() > 0)
    {
        //open setings first elemetn
        int numberFirstElement = ui->tableWidget_availebleWidgets->row(iteams[0]);

        SettingsAbstract* firstElementSettings = listPlots[numberFirstElement].first->gedSettings();

        //show then edit
        SettingsDialog *sett = new SettingsDialog(firstElementSettings);
        sett->show();
        sett->exec();

        //apply setings for all next elements
        for(int i = 1; i < iteams.size(); i++)
        {
            int numberElement = ui->tableWidget_availebleWidgets->row(iteams[i]);
            listPlots[numberElement].first->gedSettings()->setSettings(firstElementSettings);
        }
    }
}

