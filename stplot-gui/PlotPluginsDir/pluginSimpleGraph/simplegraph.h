#ifndef SIMPLEGRAPH_H
#define SIMPLEGRAPH_H

#include <QWidget>
#include "plotinterface.h"

namespace Ui {
class SimpleGraph;
}

class SimpleGraph : public PlotWidgetAbstract
{
    Q_OBJECT

public:
    explicit SimpleGraph(QWidget *parent = nullptr);
    ~SimpleGraph();
    void addPlot(QString plotName);
    void deletePlot(QString plotName);
    bool plotVar(QString plotName, QVector<VarValue> values);
    void setName(QString name);
    QString getName();

private:
    Ui::SimpleGraph *ui;
    QString name;
};

class PluginSimpleGraph : public QObject, PlotWidgetInterfacePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.S.debuger.SimpleGraph" FILE "pluginSimpleGraph.json")
    Q_INTERFACES(PlotWidgetInterfacePlugin)

public:
    PlotWidgetAbstract* createWidgetPlot(QWidget *parent = nullptr)
    {
        return new SimpleGraph(parent);
    }
    QString getName() { return QString("SimpleGraph"); }
};

#endif // SIMPLEGRAPH_H
