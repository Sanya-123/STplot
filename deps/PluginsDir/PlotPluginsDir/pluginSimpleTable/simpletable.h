#ifndef SIMPLETABLE_H
#define SIMPLETABLE_H

#include <QWidget>
#include "plotinterface.h"

namespace Ui {
class SimpleTable;
}

class SimpleTableSettings : public SettingsAbstract
{
    Q_OBJECT
public:
    SimpleTableSettings(QObject *parent = nullptr);
};

class SimpleTable : public PlotWidgetAbstract
{
    Q_OBJECT
public:
    explicit SimpleTable(SettingsAbstract* settings = nullptr, QWidget *parent = nullptr);
    ~SimpleTable();
    void redraw();
    void addPlot(VarChannel *varChanale);
    void deletePlot(VarChannel *varChanale);
//    bool plotVar(QString plotName, QVector<VarValue> values);
//    void setName(QString name);
//    QString getName();
    SettingsAbstract* gedSettings() {return &settings; }
    void resizeEvent(QResizeEvent *event);

public slots:
    void setViewProps(QHash<QString, QVariant> props);

private:
    int getGruph(QObject *sender, VarChannel **varChanale);

private slots:
    void doUpdatePlot();
//    void doUpdatePlot(VarChannel* varChanale, QCPGraph *gpuh);
    void updateColourPlot();
    void updateLineStyleGruph();
    void updateLineWidthGruph();
    void updateDotStyleGruph();
    void updateDisplayNameGruph();
    void settingsChanged();
    void plotSelecting();
    void clearPlot();

    void updateCellValues(int row, int column);
    void updateCellValues();
    void updatePositionScroll(int val);

//    void showPointToolTip(QMouseEvent *event);

private:
    Ui::SimpleTable *ui;
    QVector<VarChannel*> mapPlots;
//    QMap<VarChannel*,int> mapPlots;
    SimpleTableSettings settings;
    bool enColor;
    QColor colorForeground;
    QColor colorBackground;
};

class PluginSimpleTable : public QObject, PlotWidgetInterfacePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.S.debuger.SimpleTable" FILE "pluginSimpleTable.json")
    Q_INTERFACES(PlotWidgetInterfacePlugin)

public:
    PlotWidgetAbstract* createWidgetPlot(QWidget *parent = nullptr)
    {
        return new SimpleTable(&defSettings, parent);
    }
    QString getName() { return QString("SimpleTable"); }

    SettingsAbstract* gedDefauoldSettings() {return &defSettings;}

private:
    SimpleTableSettings defSettings;
};

#endif // SIMPLETABLE_H
