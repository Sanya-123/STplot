#include "varloader.h"
#include "ui_varloader.h"
#include "../varmodel.h"
#include <QLineEdit>
#include <QToolButton>
#include <iostream>



VarLoader::VarLoader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VarLoader)
{
    ui->setupUi(this);
    proxyModel = new QSortFilterProxyModel(this);
    tree_root = NULL;
    QObject::connect(ui->loadButton, &QToolButton::clicked, this, &VarLoader::open_elf);
    QObject::connect(ui->expandButton, &QToolButton::clicked, this, &VarLoader::expand_tree);
    QObject::connect(ui->collapseButton, &QToolButton::clicked, this, &VarLoader::collapse_tree);
    QObject::connect(ui->searchField, &QLineEdit::textChanged, this, &VarLoader::apply_filter);
}

VarLoader::~VarLoader()
{
    delete proxyModel;
    delete ui;
}

void VarLoader::expand_tree(){
    ui->treeView->expandAll();
}

void VarLoader::collapse_tree(){
    ui->treeView->collapseAll();
}

void VarLoader::apply_filter(const QString & text){
    proxyModel->setFilterFixedString(text);
}

void VarLoader::open_elf(){
    if (tree_root != NULL){
        ui->treeView->hide();
        ui->treeView->setModel(NULL);
        proxyModel->setSourceModel(NULL);
        varloc_delete_tree(tree_root);
    }

    tree_root = varloc_open_elf("/home/kasper/STM32CubeIDE/workspace_1.12.1/elf-test/Debug/elf-test.elf");
    if (tree_root == NULL){
        return;
    }

    VarModel *model = new VarModel(tree_root);
    proxyModel->setSourceModel(model);
    proxyModel->setRecursiveFilteringEnabled(true);
    ui->treeView->setModel(proxyModel);
    ui->treeView->show();
}
