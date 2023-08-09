#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QWidget>
#include <QSettings>
#include "plotinterface.h"

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

private:
    void loadPlugin();

private slots:
    void on_tableWidget_cellChanged(int row, int column);

    void on_pushButton_addView_clicked();

    void on_pushButton_deleteViewes_clicked();

private:
    Ui::ViewManager *ui;
    QList<PlotWidgetInterfacePlugin*> pluginsPlot;
    QList<PlotWidgetAbstract*> listPlots;
};

#endif // VIEWMANAGER_H
