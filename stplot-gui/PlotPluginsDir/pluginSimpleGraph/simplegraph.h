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
    void redraw();
//    bool plotVar(QString plotName, QVector<VarValue> values);
//    void setName(QString name);
//    QString getName();
    PlotSettingsAbstract* gedSettings() {return &settings; }

private:
    QCPGraph* getGruph(QObject *sender, VarChannel **varChanale);

private slots:
    void doUpdatePlot();
    void doUpdatePlot(VarChannel* varChanale, QCPGraph *gpuh);
    void updateColourPlot();
    void updateLineStyleGruph();
    void updateLineWidthGruph();
    void updateDotStyleGruph();
    void updateDisplayNameGruph();
    void settingsChanged();
    void plotSelecting();
    void limitAxisRange(const QCPRange & newRange, const QCPRange & oldRange);
    void showPointToolTip(QMouseEvent *event);
    void setTimescale(QWheelEvent *event);

private:
    Ui::SimpleGraph *ui;
//    QString name;
    QCustomPlot *plotWidget;
    QCPLayoutGrid *subLayout;
//    QCPTextElement *legendValues;
    QMap<VarChannel*,QCPGraph*> mapPlots;
    QMap<VarChannel*,QCPItemTracer*> mapTrackers;
    SimpleGraphSettings settings;
    bool emptyGraphs;
    float timescaleSec;
    QCPItemLine *cursor;
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
