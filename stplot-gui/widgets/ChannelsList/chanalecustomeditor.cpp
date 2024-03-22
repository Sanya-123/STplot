#include "chanalecustomeditor.h"
#include "ui_chanalecustomeditor.h"
#include <QAction>

ChanaleCustomEditor::ChanaleCustomEditor(QStringList chanaleNames, QString name, QString oldScript, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChanaleCustomEditor)
{
    ui->setupUi(this);
    QStringList maths;
    maths << "+";
    maths << "-";
    maths << "*";
    maths << "/";
    maths << "math.sin(";
    foreach (QString element, maths) {
        ui->listWidget_math->addItem(element);
    }

    foreach (QString element, chanaleNames) {
        ui->listWidget_names->addItem(element);
    }

    connect(ui->listWidget_math, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(iteamCliced(QListWidgetItem*)));
    connect(ui->listWidget_names, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(iteamCliced(QListWidgetItem*)));

    ui->lineEdit_name->setText(name);
    ui->textEdit_script->setText(oldScript);

    if(!name.isEmpty())
        ui->lineEdit_name->setDisabled(true);
}

ChanaleCustomEditor::~ChanaleCustomEditor()
{
    delete ui;
}

QString ChanaleCustomEditor::getName()
{
    return ui->lineEdit_name->text();
}

QString ChanaleCustomEditor::getScipt()
{
    return ui->textEdit_script->toPlainText();
}

void ChanaleCustomEditor::setScript(QString script)
{
    ui->textEdit_script->setText(script);
}

void ChanaleCustomEditor::updateChanaleNames(QStringList chanaleNames)
{
    ui->listWidget_names->clear();
    foreach (QString element, chanaleNames) {
        ui->listWidget_names->addItem(element);
    }
}

void ChanaleCustomEditor::iteamCliced(QListWidgetItem* iteam)
{
    ui->textEdit_script->textCursor().insertText(iteam->text());
    ui->textEdit_script->setFocus();
}
