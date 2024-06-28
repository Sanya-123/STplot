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
    mapSettingsDefauold["console.font"] = QFont("Sans Serif", 12);
    mapSettingsDefauold["console.autoclean"] = false;
    mapSettingsDefauold["editor.font"] = QFont();
    mapSettingsDefauold["editor.highlighter.keyword.color"] = QColor(Qt::darkBlue);
    mapSettingsDefauold["editor.highlighter.class.color"] = QColor(Qt::darkMagenta);
    mapSettingsDefauold["editor.highlighter.coment.color"] = QColor(Qt::red);
    mapSettingsDefauold["editor.highlighter.quotation.color"] = QColor(Qt::darkGreen);
    mapSettingsDefauold["editor.highlighter.function.color"] = QColor(Qt::blue);
    mapSettingsDefauold["script.isInfinity"] = false;
    mapSettingsDefauold["script.isterationRepear"] = 1.0;

    QFont highlighterFont("Sans Serif");
    highlighterFont.setBold(true);
    mapSettingsDefauold["editor.highlighter.keyword.font"] = highlighterFont;
    mapSettingsDefauold["editor.highlighter.class.font"] = highlighterFont;
    highlighterFont.setBold(false);
    mapSettingsDefauold["editor.highlighter.coment.font"] = highlighterFont;
    mapSettingsDefauold["editor.highlighter.quotation.font"] = highlighterFont;
    highlighterFont.setItalic(true);
    mapSettingsDefauold["editor.highlighter.function.font"] = highlighterFont;

    restoreDefoultSetings();
}

TestingScript::TestingScript(SettingsAbstract *settings, QWidget *parent) :
    PlotWidgetAbstract(parent),
    ui(new Ui::TestingScript), iterationRepeated(0), infinityRun(false)
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
    connect(this, SIGNAL(abortRun()), scriptExecutor, SLOT(stopScript()));
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
    helpFunctions << "print(";
    helpFunctions << "Math.sqrt(";
    helpFunctions << "Math.sin(";
    helpFunctions << "Math.cos(";
    helpFunctions << "function myFunction(a, b) { return a + b; }";
    helpFunctions << "delay(<delay_ms>)";
    helpFunctions << "setValue(\"<name>\",value)";
    helpFunctions << "getValue(\"<name>\")";
    helpFunctions << "if (typeof <variable> === 'undefined')";
    ui->listWidget_names->addItems(helpFunctions);
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
    if(iterationRepeated)
        iterationRepeated--;
    if(infinityRun || iterationRepeated)
    {
        threadScriptExecutor.quit();
        threadScriptExecutor.wait();
        if(autocleanConsole)
            ui->plainTextEdit_console->clear();
        threadScriptExecutor.start();
    }
    else
    {
        threadScriptExecutor.quit();
        threadScriptExecutor.wait();
        ui->pushButton_run->setText("run");
    }

}

void TestingScript::settingsChanged()
{
    QMap<QString, QVariant> map = settings.getSettingsMap();
    ui->splitter->restoreState(map["script.widget.spliterState"].toByteArray());
    ui->plainTextEdit_scrip->setPlainText(map["script.currentScript"].toString());
    ui->comboBox_scripts->clear();
    ui->comboBox_scripts->addItems(map["script.fileScripts"].toStringList());
    ui->comboBox_scripts->setCurrentText(map["script.currentFileScripts"].toString());

    ui->plainTextEdit_console->setFont(map["console.font"].value<QFont>());
    ui->plainTextEdit_scrip->setFont(map["editor.font"].value<QFont>());

    ui->checkBox_infinityRun->setChecked(map["script.isInfinity"].toBool());
    ui->spinBox_iterationRepat->setValue((int)(map["script.isterationRepear"].toDouble()));

    autocleanConsole = map["console.autoclean"].toBool();

//    QPalette p = ui->plainTextEdit_console->palette();
//    p.setColor(QPalette::Active, QPalette::Base, Qt::green);
//    ui->plainTextEdit_console->setPalette(p);
//    ui->plainTextEdit_console->setBackgroundVisible(false);
//    ui->plainTextEdit_console->repaint();
//    QTextCharFormat fmt;
//    fmt.setBackground(QBrush(Qt::green));
//    ui->plainTextEdit_console->mergeCurrentCharFormat(fmt);

    //update highlight color
    highlighter->setHighlighColor(Highlighter::HighlighKeyword,
                                  map["editor.highlighter.keyword.color"].value<QColor>());

    highlighter->setHighlighColor(Highlighter::HighlighClass,
                                  map["editor.highlighter.class.color"].value<QColor>());

    highlighter->setHighlighColor(Highlighter::HighlighComent,
                                  map["editor.highlighter.coment.color"].value<QColor>());

    highlighter->setHighlighColor(Highlighter::HighlighQuotation,
                                  map["editor.highlighter.quotation.color"].value<QColor>());

    highlighter->setHighlighColor(Highlighter::HighlighFunctions,
                                  map["editor.highlighter.function.color"].value<QColor>());

    //update highlight font
    highlighter->setHighlighFont(Highlighter::HighlighKeyword,
                                  map["editor.highlighter.keyword.font"].value<QFont>());

    highlighter->setHighlighFont(Highlighter::HighlighClass,
                                  map["editor.highlighter.class.font"].value<QFont>());

    highlighter->setHighlighFont(Highlighter::HighlighComent,
                                  map["editor.highlighter.coment.font"].value<QFont>());

    highlighter->setHighlighFont(Highlighter::HighlighQuotation,
                                  map["editor.highlighter.quotation.font"].value<QFont>());

    highlighter->setHighlighFont(Highlighter::HighlighFunctions,
                                  map["editor.highlighter.function.font"].value<QFont>());

    highlighter->rehighlight();
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
        infinityRun = 0;
        iterationRepeated = 0;
        emit abortRun();
    }
    else
    {
        QScriptSyntaxCheckResult res = scriptExecutor->setScript(ui->plainTextEdit_scrip->toPlainText());
        if(res.state() == QScriptSyntaxCheckResult::Valid)
        {
            infinityRun = ui->checkBox_infinityRun->isChecked();
            iterationRepeated = ui->spinBox_iterationRepat->value();
            this->settings.updateValue("script.isInfinity", infinityRun);
            this->settings.updateValue("script.isterationRepear", iterationRepeated*1.0);
            scriptExecutor->setVarChanales(mapPlots);
            if(autocleanConsole)
                ui->plainTextEdit_console->clear();
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

