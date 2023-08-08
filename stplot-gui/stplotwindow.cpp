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

    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
    QVBoxLayout* Layout = new QVBoxLayout(ui->dockContainer);
    Layout->setContentsMargins(QMargins(0, 0, 0, 0));
    m_DockManager = new ads::CDockManager(ui->dockContainer);
    Layout->addWidget(m_DockManager);

    Channels* channels_view = new Channels();
    ads::CDockWidget* DockWidget1 = new ads::CDockWidget("Channels");
    DockWidget1->setWidget(channels_view);

    VarLoader* varloader = new VarLoader();
    ads::CDockWidget* DockWidget2 = new ads::CDockWidget("VarLoader");
    DockWidget2->setWidget(varloader);

    // Add the toggleViewAction of the dock widget to the menu to give
    // the user the possibility to show the dock widget if it has been closed
    ui->menuView->addAction(DockWidget1->toggleViewAction());
    ui->menuView->addAction(DockWidget2->toggleViewAction());


    m_DockManager->addDockWidget(ads::RightDockWidgetArea, DockWidget1);
//    m_DockManager->addDockWidget(ads::LeftDockWidgetArea, DockWidget2);
    m_DockManager->addDockWidgetFloating(DockWidget2);
    DockWidget2->toggleViewAction();

    QObject::connect(varloader, &VarLoader::variable_added, channels_view, &Channels::add_channel);
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
    sl = stlink_open_usb(UDEBUG, CONNECT_HOT_PLUG, NULL, 100000);
    // if (sl == NULL){
    //     return(-1);
    // }
}
