#include "testingscript.h"
#include "ui_testingscript.h"
#include <QFileDialog>
#include <QMessageBox>

TestingScriptSettings::TestingScriptSettings(QObject *parent) : SettingsAbstract(parent)
{
    mapSettingsDefauold["script.fileScripts"] = QStringList();
    mapSettingsDefauold["script.currentFileScripts"] = QString();
    mapSettingsDefauold["script.currentScript"] = QString();
    mapSettingsDefauold["script.widget.spliterState"] = QByteArray();
    restoreDefoultSetings();
}

TestingScript::TestingScript(SettingsAbstract *settings, QWidget *parent) :
    PlotWidgetAbstract(parent),
    ui(new Ui::TestingScript)
{
    ui->setupUi(this);

    //code highter connf
    highlighter = new Highlighter(ui->plainTextEdit_scrip->document());


    //script executor
    scriptExecutor = new ScriptExecutor;
    scriptExecutor->moveToThread(&threadScriptExecutor);
    connect(&threadScriptExecutor, SIGNAL(started()), scriptExecutor, SLOT(executeScript()));
    connect(scriptExecutor, SIGNAL(printMessadge(QString)), ui->plainTextEdit_console, SLOT(appendPlainText(QString)), Qt::QueuedConnection);
    connect(scriptExecutor, SIGNAL(finishExecScript()), this, SLOT(finishExecScript()), Qt::QueuedConnection);
//    connect(&threadScriptExecutor, &QThread::finished, this, [=](){ui->pushButton_run->setEnabled(true);});

    //settings
    connect(&this->settings, SIGNAL(settingsUpdated()), this, SLOT(settingsChanged()));
    this->settings.setSettings(settings);
    //some connections for save settings
    connect(ui->splitter, &QSplitter::splitterMoved, this, [=]() {
        this->settings.updateValue("script.widget.spliterState", ui->splitter->saveState());
    });
    connect(ui->comboBox_scripts, &QComboBox::editTextChanged, this, [=]() {
        this->settings.updateValue("script.currentFileScripts", ui->comboBox_scripts->currentText());
        QStringList fileScripts;
        for(int i = 0; i < ui->comboBox_scripts->count(); i++)
            fileScripts.append(ui->comboBox_scripts->itemText(i));
        this->settings.updateValue("script.fileScripts", fileScripts);
    });

    connect(ui->plainTextEdit_scrip, &QPlainTextEdit::textChanged, this, [=]() {
        this->settings.updateValue("script.currentScript", ui->plainTextEdit_scrip->toPlainText());
    });

    //init help functions
    helpFunctions;
    helpFunctions << "+";
    helpFunctions << "-";
    helpFunctions << "*";
    helpFunctions << "/";
    helpFunctions << "main.msg = \"<text>\"";
    helpFunctions << "Math.sqrt(";
    helpFunctions << "Math.sin(";
    helpFunctions << "Math.cos(";
    helpFunctions << "function myFunction(a, b) { return a + b; }";
    helpFunctions << "delay(<delay_ms>)";
    helpFunctions << "setValue(\"<name>\",value)";
    helpFunctions << "getValue(\"<name>\")";
    ui->listWidget_names->addItems(helpFunctions);

//    connect(ui->listWidget_names, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(iteamCliced(QListWidgetItem*)));

}

TestingScript::~TestingScript()
{
    finishExecScript();
    delete ui;
}

void TestingScript::redraw()
{

}

void TestingScript::addPlot(VarChannel *varChanale)
{
    if(mapPlots.contains(varChanale))//if gpuh alredy maked
        return;

//    ui->listWidget_names->addItem();

    mapPlots.append(varChanale);
}

void TestingScript::deletePlot(VarChannel *varChanale)
{
    if(!mapPlots.contains(varChanale))//if is note created
        return;

//    ui->listWidget_names->

    int numberRow = mapPlots.indexOf(varChanale);
    mapPlots.remove(numberRow);
}

void TestingScript::finishExecScript()
{
    threadScriptExecutor.quit();
    threadScriptExecutor.wait();
    ui->pushButton_run->setText("run");
}

void TestingScript::settingsChanged()
{
    QMap<QString, QVariant> map = settings.getSettingsMap();
    ui->splitter->restoreState(map["script.widget.spliterState"].toByteArray());
    ui->plainTextEdit_scrip->setPlainText(map["script.currentScript"].toString());
    ui->comboBox_scripts->clear();
    ui->comboBox_scripts->addItems(map["script.fileScripts"].toStringList());
    ui->comboBox_scripts->setCurrentText(map["script.currentFileScripts"].toString());
}

void TestingScript::on_pushButton_selectScrip_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select file"), ui->comboBox_scripts->currentText(), "Test script (*.ts);;Text files (*.txt);;All (*)");
    if(!file.isEmpty())
        ui->comboBox_scripts->setCurrentText(file);
}

void TestingScript::on_pushButton_loadScript_clicked()
{
    QFile fileScript(ui->comboBox_scripts->currentText());
    if(fileScript.open(QIODevice::ReadOnly))
    {
        ui->plainTextEdit_scrip->clear();
        ui->plainTextEdit_scrip->setPlainText(fileScript.readAll());
        fileScript.close();
    }
}

void TestingScript::on_pushButton_run_clicked()
{
    if(threadScriptExecutor.isRunning())
    {
        finishExecScript();
    }
    else
    {
        QScriptSyntaxCheckResult res = scriptExecutor->setScript(ui->plainTextEdit_scrip->toPlainText());
        if(res.state() == QScriptSyntaxCheckResult::Valid)
        {
            scriptExecutor->setVarChanales(mapPlots);
            threadScriptExecutor.start();
            ui->pushButton_run->setText("stop");
        }
        else
        {
            QMessageBox::critical(this, "Error script",
                                  tr("On line %1 Error:").arg(res.errorLineNumber()) + res.errorMessage());
        }
    }
}

void TestingScript::on_pushButton_saveScript_clicked()
{
    QString file = QFileDialog::getSaveFileName(this, "Save script", ui->comboBox_scripts->currentText(), "Test script (*.ts);;Text files (*.txt);;All (*)");
    if(!file.isEmpty())
    {
        QFile saveFile(file);
        if(saveFile.open(QIODevice::WriteOnly))
        {
            saveFile.write(ui->plainTextEdit_scrip->toPlainText().toLocal8Bit());
            ui->comboBox_scripts->setCurrentText(file);
        }
    }
}

void TestingScript::on_listWidget_names_itemClicked(QListWidgetItem *item)
{
    ui->plainTextEdit_scrip->textCursor().insertText(item->text());
    ui->plainTextEdit_scrip->setFocus();
}

