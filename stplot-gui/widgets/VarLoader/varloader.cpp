#include "varloader.h"
#include "ui_varloader.h"
#include <QLineEdit>
#include <QToolButton>
#include <QPushButton>
#include <iostream>
#include <QFileDialog>

VarLoader::VarLoader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VarLoader)
{
    ui->setupUi(this);
    proxyModel = new QSortFilterProxyModel(this);
    varModel = new VarModel();
    QObject::connect(ui->loadButton, &QPushButton::clicked, this, &VarLoader::open_elf);
    QObject::connect(ui->addButton, &QPushButton::clicked, this, &VarLoader::add_variables);
    QObject::connect(ui->expandButton, &QToolButton::clicked, this, &VarLoader::expand_tree);
    QObject::connect(ui->collapseButton, &QToolButton::clicked, this, &VarLoader::collapse_tree);
    QObject::connect(ui->searchField, &QLineEdit::textChanged, this, &VarLoader::apply_filter);
}

VarLoader::~VarLoader()
{
    if (varModel->getModelRoot() != NULL){
        ui->treeView->setModel(NULL);
        proxyModel->setSourceModel(NULL);
        varloc_delete_tree(varModel->getModelRoot());
    }
    delete proxyModel;
    delete ui;
    delete varModel;
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


void VarLoader::add_variables(){
    QMap<varloc_node_t*, bool> &selection_map = varModel->get_selected_nodes();
    QMap<varloc_node_t*, bool>::const_iterator i = selection_map.constBegin();
    while (i != selection_map.constEnd()) {
        if (i.value() == true){
            varloc_node_t* node = i.key();
            if (node->var_type == BASE){
                emit variable_added(node);
            }
        }
        ++i;
    }
    varModel->deselect_all();

}


#define TESTING 1
void VarLoader::open_elf(){
#if !TESTING
    QString fileName = QFileDialog::getOpenFileName(this,
                    tr("ELF file"), NULL, tr("ELF Files (*.elf)"));
    if (fileName.isEmpty()){
        return;
    }
#else
    QString fileName = NULL;
#endif
    this->load_variables(fileName);
}


void VarLoader::load_variables(const QString & fileName){

#if TESTING
    char* filepath = "/home/kasper/STM32CubeIDE/workspace_1.12.1/elf-test/Debug/elf-test.elf";
#else
    QByteArray ba = fileName.toLocal8Bit();
    char *filepath = ba.data();
#endif
    if (varModel->getModelRoot() != NULL){
        ui->treeView->hide();
        ui->treeView->setModel(NULL);
        proxyModel->setSourceModel(NULL);
        varModel->setModelRoot(NULL);
        varloc_delete_tree(varModel->getModelRoot());
    }

    varloc_node_t* tree_root = varloc_open_elf(filepath);
    if (tree_root == NULL){
        return;
    }
    ui->label->setText(fileName);
    varModel->setModelRoot(tree_root);
    proxyModel->setSourceModel(varModel);
    proxyModel->setRecursiveFilteringEnabled(true);
    ui->treeView->setModel(proxyModel);
    ui->treeView->show();
}
