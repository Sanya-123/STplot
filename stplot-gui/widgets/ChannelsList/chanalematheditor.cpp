#include "chanalematheditor.h"
#include "ui_chanalematheditor.h"
#include <QAction>
#include "varchannel.h"

ChanaleMathEditor::ChanaleMathEditor(QStringList chanaleNames, QString name, QString oldScript, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChanaleMathEditor)
{
    ui->setupUi(this);
    QStringList maths;
    maths << "+";
    maths << "-";
    maths << "*";
    maths << "/";
    maths << "Math.sqrt(";
    maths << "Math.sin(";
    maths << "Math.cos(";
    maths << "function myFunction(a, b) { return a + b; }";
    maths << "prevValue(\"<name>\",<delay>)";
    foreach (QString element, maths) {
        ui->listWidget_math->addItem(element);
    }

    updateChanaleNames(chanaleNames);

    connect(ui->listWidget_math, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(iteamCliced(QListWidgetItem*)));
    connect(ui->listWidget_names, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(iteamCliced(QListWidgetItem*)));

    ui->lineEdit_name->setText(name);
    ui->textEdit_script->setText(oldScript);

    if(!name.isEmpty())
        ui->lineEdit_name->setDisabled(true);
}

ChanaleMathEditor::~ChanaleMathEditor()
{
    delete ui;
}

QString ChanaleMathEditor::getName()
{
    return ui->lineEdit_name->text();
}

QString ChanaleMathEditor::getScipt()
{
    return ui->textEdit_script->toPlainText();
}

void ChanaleMathEditor::setScript(QString script)
{
    ui->textEdit_script->setText(script);
}

void ChanaleMathEditor::updateChanaleNames(QStringList chanaleNames)
{
    ui->listWidget_names->clear();
    foreach (QString element, chanaleNames) {
        REPLEASE_DOT_VAR_NAME(element);
        ui->listWidget_names->addItem(element);
    }
}

void ChanaleMathEditor::iteamCliced(QListWidgetItem* iteam)
{
    ui->textEdit_script->textCursor().insertText(iteam->text());
    ui->textEdit_script->setFocus();
}
