#include "viewmanager.h"
#include "ui_viewmanager.h"

ViewManager::ViewManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewManager)
{
    ui->setupUi(this);
}

ViewManager::~ViewManager()
{
    delete ui;
}
