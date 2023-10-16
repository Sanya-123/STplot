#ifndef SIMPLEGRAPH_H
#define SIMPLEGRAPH_H

#include <QWidget>
#include "plotinterface.h"
#include "qcustomplot.h"

namespace Ui {
class SimpleGraph;
}

class SimpleGraphSettings : public PlotSettingsAbstract
{
    Q_OBJECT
public:
    SimpleGraphSettings(QObject *parent = nullptr);
};

class SimpleGraph : public PlotWidgetAbstract
{
    Q_OBJECT

public:
    explicit SimpleGraph(PlotSettingsAbstract* settings = nullptr, QWidget *parent = nullptr);
    ~SimpleGraph();
    void addPlot(VarChannel *varChanale);
    void deletePlot(VarChannel *varChanale);
//    bool plotVar(QString plotName, QVector<VarValue> values);
//    void setName(QString name);
//    QString getName();
    PlotSettingsAbstract* gedSettings() {return &settings; }

private:
    QCPGraph* getGruph(QObject *sender, VarChannel **varChanale);

private slots:
    void doUpdatePlot();
    void updateColourPlot();
    void updateLineStyleGruph();
    void updateLineWidthGruph();
    void updateDotStyleGruph();
    void settingsChanged();

private:
    Ui::SimpleGraph *ui;
//    QString name;
    QCustomPlot *plotWidget;
    QMap<VarChannel*,QCPGraph*> mapPlots;
    SimpleGraphSettings settings;
};

class PluginSimpleGraph : public QObject, PlotWidgetInterfacePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.S.debuger.SimpleGraph" FILE "pluginSimpleGraph.json")
    Q_INTERFACES(PlotWidgetInterfacePlugin)

public:
    PlotWidgetAbstract* createWidgetPlot(QWidget *parent = nullptr)
    {
        return new SimpleGraph(&defSettings, parent);
    }
    QString getName() { return QString("SimpleGraph"); }

    PlotSettingsAbstract* gedDefauoldSettings() {return &defSettings;}

private:
    SimpleGraphSettings defSettings;
};

#endif // SIMPLEGRAPH_H
