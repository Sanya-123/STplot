#ifndef STPLOTWINDOW_H
#define STPLOTWINDOW_H

#include <QMainWindow>
#include "DockManager.h"

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
    // virtual void closeEvent(QCloseEvent *event) override;

private:
    Ui::STPlotWindow *ui;
    ads::CDockManager* m_DockManager;
};





#endif // STPLOTWINDOW_H
