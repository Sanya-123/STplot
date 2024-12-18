#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QWidget>
#include <QSettings>
#include "plotinterface.h"
#include "DockManager.h"
#include "channels.h"

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

#define MINIMUM_PLUGIN_PLOT_HEADER_VERSION          0x00000000

namespace Ui {
class ViewManager;
}

class ViewManager : public QWidget
{
    Q_OBJECT

public:
    explicit ViewManager(QWidget *parent = nullptr);
    ~ViewManager();

    void saveSettings(QSettings *settings);
    void restoreSettings(QSettings *settings);

    void setDockContainer(ads::CDockManager *newDockContainer);
    void setMenuView(QMenu *newMenuView);
    void setChanales(Channels *newChanales);

public slots:
    void updateAllViews();

signals:
    //TODO add update list graphsignals

private:

private slots:
    void addPotToPlot(VarChannel* var, int numPlot, bool en);
    PlotWidgetAbstract *addPlot(PlotWidgetInterfacePlugin* plotType, QString name);
    void on_tableWidget_availebleWidgets_cellChanged(int row, int column);

    void on_pushButton_addView_clicked();

    void on_pushButton_deleteViewes_clicked();

    void on_pushButton_open_def_setings_clicked();

    void on_pushButton_open_selected_plot_setings_clicked();

private:
    Ui::ViewManager *ui;
    QList<PlotWidgetInterfacePlugin*> pluginsPlot;
    QList<QPair<PlotWidgetAbstract*, ads::CDockWidget*>> listPlots;//list ow widgets plot and docker widgets for remove
    QList<QPair<PlotWidgetAbstract*, QString>> listPlotParants;//list for save and restore plot
    ads::CDockManager *dockContainer;
    QMenu *menuView;
    Channels *chanales;
};

#endif // VIEWMANAGER_H
