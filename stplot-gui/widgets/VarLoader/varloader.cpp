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
    varModel = new VarModel(this);
    connect(ui->pushButton_selectFile, &QPushButton::clicked, this, &VarLoader::open_elf);
    connect(ui->loadButton, &QPushButton::clicked, this, &VarLoader::load_elf);
    connect(ui->addButton, &QPushButton::clicked, this, &VarLoader::add_variables);
    connect(ui->expandButton, &QToolButton::clicked, this, &VarLoader::expand_tree);
    connect(ui->collapseButton, &QToolButton::clicked, this, &VarLoader::collapse_tree);
    connect(ui->searchField, &QLineEdit::textChanged, this, &VarLoader::apply_filter);
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

void VarLoader::saveSettings(QSettings *settings)
{
    settings->setValue("elffile", ui->lineEdit_file->text());
}

void VarLoader::restoreSettings(QSettings *settings)
{
    ui->lineEdit_file->setText(settings->value("elffile").toString());
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


void VarLoader::add_variables()
{
    QMap<varloc_node_t*, bool> &selection_map = varModel->get_selected_nodes();
    QMap<varloc_node_t*, bool>::const_iterator i = selection_map.constBegin();
    while (i != selection_map.constEnd()) {
        if (i.value() == true){
            varloc_node_t* node = i.key();
            if ((node->var_type == BASE)
                || (node->var_type == ENUM)
            ){
                emit variable_added(node);
            }
        }
        ++i;
    }
    varModel->deselect_all();

}

void VarLoader::open_elf()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                    tr("ELF file"), ui->lineEdit_file->text(), tr("ELF Files (*.elf)"));
    if (!fileName.isEmpty())
    {
        ui->lineEdit_file->setText(fileName);
    }
}

void VarLoader::load_elf()
{
    load_variables(ui->lineEdit_file->text());
}

void VarLoader::load_variables(const QString & fileName)
{
    QByteArray ba = fileName.toLocal8Bit();
    char *filepath = ba.data();
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
    varModel->setModelRoot(tree_root);
    proxyModel->setSourceModel(varModel);
    proxyModel->setRecursiveFilteringEnabled(true);
    ui->treeView->setModel(proxyModel);
    ui->treeView->show();
}
