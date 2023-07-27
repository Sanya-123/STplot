#include "stplotwindow.h"
#include "./ui_stplotwindow.h"

#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QTreeView>
#include <QFile>

#include <iostream>
#include <format>
#include "varmodel.h"
#include "varloader.h"
//#include <elfio/elfio_dump.hpp>

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

    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
    QVBoxLayout* Layout = new QVBoxLayout(ui->dockContainer);
    Layout->setContentsMargins(QMargins(0, 0, 0, 0));
    m_DockManager = new ads::CDockManager(ui->dockContainer);
    Layout->addWidget(m_DockManager);

    // Create example content label - this can be any application specific
    // widget
    QLabel* l = new QLabel();
    l->setWordWrap(true);
    l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

    // Create a dock widget with the title Label 1 and set the created label
    // as the dock widget content
    ads::CDockWidget* DockWidget = new ads::CDockWidget("Test");
    DockWidget->setWidget(l);

    VarLoader* varloader = new VarLoader();
    ads::CDockWidget* DockWidget2 = new ads::CDockWidget("VarLoader");
    DockWidget2->setWidget(varloader);

    // Add the toggleViewAction of the dock widget to the menu to give
    // the user the possibility to show the dock widget if it has been closed
    ui->menuView->addAction(DockWidget->toggleViewAction());
    ui->menuView->addAction(DockWidget2->toggleViewAction());


    m_DockManager->addDockWidget(ads::RightDockWidgetArea, DockWidget);
    m_DockManager->addDockWidget(ads::LeftDockWidgetArea, DockWidget2);
}

STPlotWindow::~STPlotWindow()
{
    delete ui;
}


void STPlotWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
    if (m_DockManager)
    {
        m_DockManager->deleteLater();
    }
}

void STPlotWindow::connect(){
    stlink_t* sl = NULL;
    enum connect_type connect = CONNECT_HOT_PLUG;
    int32_t freq = 100000;
    sl = stlink_open_usb(UDEBUG, connect, NULL, freq);
    // if (sl == NULL){
    //     return(-1);
    // }
}

#if 0
static uint32_t sym_n;
void STPlotWindow::insert_var_row(varloc_node_t* node){
    if(!(node->name)){
        return;
    }
//    QTableWidgetItem *nameItem = new QTableWidgetItem(QString::fromStdString(node->name));
//    this->ui->symbolTable->setItem(sym_n, 0, nameItem);
//    QTableWidgetItem *valueItem = new QTableWidgetItem(QString::fromStdString(node->ctype_name));
//    this->ui->symbolTable->setItem(sym_n, 1, valueItem);
//    sym_n++;
}


void STPlotWindow::for_each_var_loop(varloc_node_t* root, void (STPlotWindow::*func)(varloc_node_t*)){
    (this->*(func))(root);
    if (root->child != NULL){
        this->for_each_var_loop(root->child, func);
    }
    if (root->next != NULL){
        this->for_each_var_loop(root->next, func);
    }
}

void STPlotWindow::apply_filter(const QString & text){
    proxyModel->setFilterFixedString(text);
//    this->ui->treeView->expandAll();
}

void STPlotWindow::open_elf(){


    std::cout << "Start variable locator" << std::endl;

    varloc_node_t* root = varloc_open_elf("/home/kasper/STM32CubeIDE/workspace_1.12.1/elf-test/Debug/elf-test.elf");
//    this->for_each_var_loop(root, &STPlotWindow::insert_var_row);
    VarModel *model = new VarModel(root);


    proxyModel->setSourceModel(model);
    proxyModel->setRecursiveFilteringEnabled(true);

    this->ui->treeView->setModel(proxyModel);
    this->ui->treeView->show();

    std::cout << "End variable locator" << std::endl;
}
#endif
