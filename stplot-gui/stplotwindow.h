#ifndef STPLOTWINDOW_H
#define STPLOTWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include "DockManager.h"
extern "C" {
#include "varloc.h"
}
QT_BEGIN_NAMESPACE
namespace Ui { class STPlotWindow; }
QT_END_NAMESPACE

class STPlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    STPlotWindow(QWidget *parent = nullptr);
    ~STPlotWindow();

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    Ui::STPlotWindow *ui;
    ads::CDockManager* m_DockManager;

public slots:
    void connect();

};





#endif // STPLOTWINDOW_H
