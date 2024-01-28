#include "varloader.h"
#include "ui_varloader.h"
#include <QLineEdit>
#include <QToolButton>
#include <QPushButton>
#include <iostream>
#include <QFileDialog>
#include <QMessageBox>

VarLoader::VarLoader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VarLoader)
{
    ui->setupUi(this);
    proxyModel = new QSortFilterProxyModel(this);
    varModel = new VarModel(this);
    connect(ui->pushButton_selectFile, &QPushButton::clicked, this, &VarLoader::openElf);
    connect(ui->loadButton, &QPushButton::clicked, this, &VarLoader::loadElf);
    connect(ui->addButton, &QPushButton::clicked, this, &VarLoader::addVariables);
    connect(ui->expandButton, &QToolButton::clicked, this, &VarLoader::expandTree);
    connect(ui->collapseButton, &QToolButton::clicked, this, &VarLoader::collapseTree);
    connect(ui->searchField, &QLineEdit::textChanged, this, &VarLoader::applyFilter);
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
    //maybe clean some stuff
    ui->lineEdit_file->setText(settings->value("elffile").toString());
}

void VarLoader::expandTree(){
    ui->treeView->expandAll();
}

void VarLoader::collapseTree(){
    ui->treeView->collapseAll();
}

void VarLoader::applyFilter(const QString & text){
    proxyModel->setFilterFixedString(text);
}
bool VarLoader::isElfLoaded(){
    return (varModel->getModelRoot() != NULL);
}


void VarLoader::addVariables()
{
    QMap<varloc_node_t*, bool> &selection_map = varModel->get_selected_nodes();
    QMap<varloc_node_t*, bool>::const_iterator i = selection_map.constBegin();
    while (i != selection_map.constEnd()) {
        if (i.value() == true){
            varloc_node_t* node = i.key();
            if ((node->var_type == BASE)
                || (node->var_type == ENUM)
            ){
                emit variableAdded(node);
            }
        }
        ++i;
    }
    varModel->deselect_all();
    ui->treeView->viewport()->update();
}

void VarLoader::openElf()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                    tr("ELF file"), ui->lineEdit_file->text(), tr("ELF Files (*.elf)"));
    if (!fileName.isEmpty())
    {
        watcher.removePath(ui->lineEdit_file->text());
        ui->lineEdit_file->setText(fileName);
    }
}

void VarLoader::loadElf()
{
    loadVariables(ui->lineEdit_file->text());
}

void VarLoader::updateElf(const QString& path)
{
    QMessageBox msgBox;
    msgBox.setText("The ELF file was modified.");
    msgBox.setInformativeText("Do you want to reload variables?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Ok){
        watcher.removePath(ui->lineEdit_file->text());
        loadVariables(ui->lineEdit_file->text());
    }
}

void VarLoader::loadVariables(const QString & fileName)
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

    watcher.addPath(fileName);
    QObject::connect(&watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(updateElf(const QString&)));

    emit variablesUpdated(tree_root);
}
