#include "varloader.h"
#include "ui_varloader.h"
#include <QLineEdit>
#include <QToolButton>
#include <QPushButton>
#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>

VarLoader::VarLoader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VarLoader)
{
    ui->setupUi(this);

    pluginsReader = loadPlugin<VarReadInterfacePlugin>(MINIMUM_PLUGIN_READER_HEADER_VERSION, VAR_READER_INTERFACE_HEADER_VERSION);

    allowReadFiles.clear();
    allowWriteFiles.clear();
    foreach (VarReadInterfacePlugin *plugin, pluginsReader) {
        mapPluginFile[plugin->getFileExtensions()] = plugin;
        if(plugin->allowMode() & QIODevice::ReadOnly)
        {
            allowReadFiles.append(QString("%1 (%2);;").arg(plugin->getName(), plugin->getFileExtensions()));
        }
        if(plugin->allowMode() & QIODevice::WriteOnly)
        {
            allowWriteFiles.append(QString("%1 (%2);;").arg(plugin->getName(), plugin->getFileExtensions()));
        }
    }

    //remove lst ;
    if(!allowReadFiles.isEmpty())
        allowReadFiles.remove(allowReadFiles.size()-2, 2);
    else
        ui->loadButton->setEnabled(false);
    if(!allowWriteFiles.isEmpty())
        allowWriteFiles.remove(allowWriteFiles.size()-2, 2);
    else
        ui->saveButton->setEnabled(false);

    qDebug () << "allowReadFiles:" << allowReadFiles;
    qDebug () << "allowWriteFiles:" << allowWriteFiles;


    proxyModel = new VarFilter(this);
    // proxyModel = new QSortFilterProxyModel(this);
    varModel = new VarModel(this);
    connect(ui->pushButton_selectFile, &QPushButton::clicked, this, &VarLoader::openTree);
    connect(ui->loadButton, &QPushButton::clicked, this, &VarLoader::loadTree);
    connect(ui->addButton, &QPushButton::clicked, this, &VarLoader::addVariables);
    connect(ui->expandButton, &QToolButton::clicked, this, &VarLoader::expandTree);
    connect(ui->collapseButton, &QToolButton::clicked, this, &VarLoader::collapseTree);
    connect(ui->searchField, &QLineEdit::textChanged, this, &VarLoader::applyFilter);
    connect(ui->saveButton, &QPushButton::clicked, this, &VarLoader::saveTree);
    connect(&watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(updateTree(const QString&)));
}

VarLoader::~VarLoader()
{
    if (varModel->getModelRoot() != NULL){
        ui->treeView->setModel(NULL);
        proxyModel->setSourceModel(NULL);
        varloc_delete_tree(varModel->getModelRoot());
    }
    delete proxyModel;
    delete varModel;
    delete ui;
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
    collapseTree();
    proxyModel->invalidate();
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

void VarLoader::openTree()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                    tr("Tree file"), ui->lineEdit_file->text(), allowReadFiles);
    if (!fileName.isEmpty())
    {
        watcher.removePath(ui->lineEdit_file->text());
        ui->lineEdit_file->setText(fileName);
    }
}

void VarLoader::loadTree()
{
    loadVariables(ui->lineEdit_file->text());
}

void VarLoader::saveTree()
{
    //TODO implement
}

void VarLoader::updateTree(const QString& path)
{
    QMessageBox msgBox;
    msgBox.setText("The ELF file was modified.");
    msgBox.setInformativeText("Do you want to reload variables?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes){
        watcher.removePath(ui->lineEdit_file->text());
        loadVariables(ui->lineEdit_file->text());
    }
}

void VarLoader::loadVariables(const QString & fileName)
{
    QFileInfo file_info(fileName);
    if(!file_info.exists(fileName)){
        QMessageBox msgBox;
        msgBox.setText("ELF file not found");
        msgBox.exec();
        return;
    }

    QByteArray ba = fileName.toLocal8Bit();
    char *filepath = ba.data();
    if (varModel->getModelRoot() != NULL){
        ui->treeView->hide();
        ui->treeView->setModel(NULL);
        proxyModel->setSourceModel(NULL);
        varModel->setModelRoot(NULL);
        varloc_delete_tree(varModel->getModelRoot());
    }

    varloc_node_t* tree_root = nullptr;


    QFileInfo fileInfo(fileName);
    QString _fileName = fileInfo.fileName();


    QList<QString> fileFormats = mapPluginFile.keys();
    foreach (QString format, fileFormats) {
        QRegExp rx(format);
        //TODO check QRegExp filetsr
        rx.setPatternSyntax(QRegExp::Wildcard);
        if(rx.exactMatch(_fileName))
        {
            tree_root = mapPluginFile[format]->readTree(fileName);
        }

        //if plugin is note exactMatch or plugin is note load tree check next plugin
        //it is posible taht couple plagit use same suffix but different format
        if (tree_root != nullptr)
            break;
    }


    if (tree_root == nullptr){
        return;
    }
    varModel->setModelRoot(tree_root);
    proxyModel->setSourceModel(varModel);
    proxyModel->setRecursiveFilteringEnabled(true);
    // proxyModel->setAutoAcceptChildRows(true);
    ui->treeView->setModel(proxyModel);
    ui->treeView->show();
    ui->treeView->setColumnWidth(0,300);

    watcher.addPath(fileName);

    emit variablesUpdated(tree_root);
}
