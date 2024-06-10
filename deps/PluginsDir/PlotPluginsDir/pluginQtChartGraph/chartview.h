#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QWidget>
#include "plotinterface.h"
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtWidgets/QGraphicsView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

class ChartViewSettings : public SettingsAbstract
{
    Q_OBJECT
public:
    ChartViewSettings(QObject *parent = nullptr);
};

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
    Q_OBJECT
public:
    ChartView(QChart *chart, QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void rescaleAxisX();
    void rescaleAxisY();

private:
    QPoint oldPos;
    bool leftButtonPresed;
};

//base on callout example https://code.qt.io/cgit/qt/qtcharts.git/tree/examples/charts/callout?h=5.15
class ChartViewWidget : public PlotWidgetAbstract
{
    Q_OBJECT
public:
    explicit ChartViewWidget(SettingsAbstract* settings = nullptr, QWidget *parent = nullptr);
    ~ChartViewWidget();
    void addPlot(VarChannel *varChanale);
    void deletePlot(VarChannel *varChanale);
    void redraw();
//    bool plotVar(QString plotName, QVector<VarValue> values);
//    void setName(QString name);
//    QString getName();
    SettingsAbstract* gedSettings() {return &settings; }

private:
    QLineSeries* getGruph(QObject *sender, VarChannel **varChanale);
    void restoreStyleGraphs();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void doUpdatePlot();
    void doUpdatePlot(VarChannel* varChanale, QLineSeries *gpuh);
    void updateColourPlot();
    void updateLineStyleGruph();
    void updateLineWidthGruph();
    void updateDotStyleGruph();
    void updateDisplayNameGruph();
    void settingsChanged();
    void plotSelecting();
    void plotClear();
    void rescaleAxis(bool isX);
    void rescaleAxisX();
    void rescaleAxisY();

private:
    ChartView *view;
    QChart *plotWidget;
    QDateTimeAxis *axisX;
    QValueAxis *axisY;
    QMap<VarChannel*,QLineSeries*> mapPlots;
//    QMap<VarChannel*,QCPItemTracer*> mapTrackers;
    ChartViewSettings settings;
    bool emptyGraphs;

    bool   firstRedraw;
    bool   rightMousePressed;
    QPoint lastClickPos;
    double scaleTime;
//    QCPRange lastTimeRange;
//    QCPRange lastValueRange;
    bool simpleScaleMode;
    QTime lastTime;

    bool enableOptimizerPoints;
    bool isOptimizerPoints;
};

class pluginQtChartGraph : public QObject, PlotWidgetInterfacePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.S.debuger.QtChartGraph" FILE "pluginQtChartGraph.json")
    Q_INTERFACES(PlotWidgetInterfacePlugin)

public:
    PlotWidgetAbstract* createWidgetPlot(QWidget *parent = nullptr)
    {
        return new ChartViewWidget(&defSettings, parent);
    }
    QString getName() { return QString("QtChartGraph"); }

    SettingsAbstract* gedDefauoldSettings() {return &defSettings;}

private:
    ChartViewSettings defSettings;
};

#endif // CHARTVIEW_H
