#include "viewmanager.h"
#include "ui_viewmanager.h"
#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QMenu>
#include "settingsdialog.h"
#include "stplotpluginloader.h"

ViewManager::ViewManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewManager), dockContainer(nullptr), menuView(nullptr), chanales(nullptr)
{
    ui->setupUi(this);
    pluginsPlot = loadPlugin<PlotWidgetInterfacePlugin>(MINIMUM_PLUGIN_PLOT_HEADER_VERSION, PLOT_INTERFACE_HEADER_VERSION);
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
        settings->beginWriteArray("connectedPlots");
        for (int j = 0; j < listPlotParants.size(); j++){
            settings->setArrayIndex(j);
            QTableWidgetItem* tableItem = ui->tableWidget_availebleWidgets->item(i,j+1);
            if (tableItem != nullptr){
                bool checked = tableItem->checkState() == Qt::Checked;
                settings->setValue("enPlot", checked);
            }else{
                settings->setValue("enPlot", false);
            }
        }
        settings->endArray();
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
    // another loop for restoring checkboxes after table is filled by addPlot calls above
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        int nConnectedPlots = settings->beginReadArray("connectedPlots");
        for (int j = 0; j < nConnectedPlots; ++j) {
            settings->setArrayIndex(j);
            if(settings->value("enPlot").toBool()){
                ui->tableWidget_availebleWidgets->item(i,j+1)->setCheckState(Qt::Checked);
            }
        }
        settings->endArray();
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

    //add action open sitings to title bar
    QAction * actionConfigSettings = new QAction("settings", widgetDocker);
    connect(actionConfigSettings, &QAction::triggered, this, [=](){
        //show then edit
        SettingsDialog *sett = new SettingsDialog(widgetPlot->gedSettings());
        sett->show();
        sett->exec();
        delete sett;
    });

    QList<QAction*> actions = widgetDocker->titleBarActions();
    actions.append(actionConfigSettings);
    widgetDocker->setTitleBarActions(actions);

    //add view to menu
    if(menuView != nullptr)
        menuView->addAction(widgetDocker->toggleViewAction());
    //add to chanal modele
    if(chanales != nullptr)
        chanales->addPlot();

    listPlots.append(qMakePair(widgetPlot, widgetDocker));
    listPlotParants.append(qMakePair(widgetPlot, plotType->getName()));

    ui->tableWidget_availebleWidgets->setRowCount(numberRow + 1);
    ui->tableWidget_availebleWidgets->setItem(numberRow, 0, new QTableWidgetItem(name));

    int columnCount = ui->tableWidget_availebleWidgets->columnCount();
    ui->tableWidget_availebleWidgets->insertColumn(columnCount);
    ui->tableWidget_availebleWidgets->setHorizontalHeaderItem(columnCount, new QTableWidgetItem(name));

    /**
     *
     * name old old old new
     * name old old old new
     * name old old old new
     * name new new new new
     *
     * needed update just new
    */
    for(int i = 0 ; i < numberRow + 1; i++)
    {
        if(i != numberRow)
        {
            QTableWidgetItem *checkBoxItem = new QTableWidgetItem();
            checkBoxItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            checkBoxItem->setCheckState(Qt::Unchecked);
            ui->tableWidget_availebleWidgets->setItem(i, numberRow + 1, checkBoxItem);

            checkBoxItem = new QTableWidgetItem();
            checkBoxItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            checkBoxItem->setCheckState(Qt::Unchecked);
            ui->tableWidget_availebleWidgets->setItem(numberRow, i + 1, checkBoxItem);
        }
    }

    return widgetPlot;
}

void ViewManager::on_tableWidget_availebleWidgets_cellChanged(int row, int column)
{
    if(row >= 0 && row < listPlots.size() && column == 0)
    {
        QString newName = ui->tableWidget_availebleWidgets->item(row, column)->text();
        listPlots[row].second->setWindowTitle(newName);
        listPlots[row].second->setObjectName(newName);
        listPlots[row].first->setWindowTitle(newName);
//        listPlots[row].second->setObjectName(newName);

        if(chanales != nullptr)
            chanales->setPlotName(row, newName);
    }
    else if (column != 0){
        bool checked = ui->tableWidget_availebleWidgets->item(row, column)->checkState();
        qDebug("cell changed %d %d %d", row, column, checked);
        if (checked){
            connect(listPlots[row].first, &PlotWidgetAbstract::viewPropsUpdated, listPlots[column - 1].first, &PlotWidgetAbstract::setViewProps);
        }
        else{
            disconnect(listPlots[row].first, &PlotWidgetAbstract::viewPropsUpdated, listPlots[column - 1].first, &PlotWidgetAbstract::setViewProps);
        }

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
        ui->tableWidget_availebleWidgets->removeColumn(numberElement + 1);
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

        delete sett;

        //apply setings for all next elements
        for(int i = 1; i < iteams.size(); i++)
        {
            int numberElement = ui->tableWidget_availebleWidgets->row(iteams[i]);
            listPlots[numberElement].first->gedSettings()->setSettings(firstElementSettings);
        }
    }
}

