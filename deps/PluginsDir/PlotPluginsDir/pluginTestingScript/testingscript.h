#ifndef TESTINGSCRIPT_H
#define TESTINGSCRIPT_H

#include <QWidget>
#include "plotinterface.h"
#include <QThread>
#include "scriptexecutor.h"
#include <QPushButton>
#include "highlighter.h"

QT_BEGIN_NAMESPACE
class QListWidgetItem;
QT_END_NAMESPACE

namespace Ui {
class TestingScript;
}

class TestingScriptSettings : public SettingsAbstract
{
    Q_OBJECT
public:
    TestingScriptSettings(QObject *parent = nullptr);
};


class TestingScript : public PlotWidgetAbstract
{
    Q_OBJECT
public:
    explicit TestingScript(SettingsAbstract* settings = nullptr, QWidget *parent = nullptr);
    ~TestingScript();
    void redraw();
    void addPlot(VarChannel *varChanale);
    void deletePlot(VarChannel *varChanale);
    SettingsAbstract* gedSettings() {return &settings; }

private slots:
    void finishExecScript();
    void settingsChanged();
    void on_pushButton_selectScrip_clicked();

    void on_pushButton_loadScript_clicked();

    void on_pushButton_run_clicked();

    void on_pushButton_saveScript_clicked();

    void on_listWidget_names_itemClicked(QListWidgetItem *item);

private:
    Ui::TestingScript *ui;
    QVector<VarChannel*> mapPlots;
    TestingScriptSettings settings;
    QThread threadScriptExecutor;
    ScriptExecutor *scriptExecutor;
    Highlighter *highlighter;
    QStringList helpFunctions;
};

class PluginTestingScript : public QObject, PlotWidgetInterfacePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.S.debuger.TestingScript" FILE "pluginTestingScript.json")
    Q_INTERFACES(PlotWidgetInterfacePlugin)

public:
    PlotWidgetAbstract* createWidgetPlot(QWidget *parent = nullptr)
    {
        return new TestingScript(&defSettings, parent);
    }
    QString getName() { return QString("TestingScript"); }

    SettingsAbstract* gedDefauoldSettings() {return &defSettings;}

private:
    TestingScriptSettings defSettings;
};

#endif // TESTINGSCRIPT_H
