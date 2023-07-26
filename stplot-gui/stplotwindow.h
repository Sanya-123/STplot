#ifndef STPLOTWINDOW_H
#define STPLOTWINDOW_H

#include <QMainWindow>
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
    void for_each_var_loop(varloc_node_t* root, void (STPlotWindow::*func)(varloc_node_t*));
    void insert_var_row(varloc_node_t* node);

private:
    Ui::STPlotWindow *ui;
    ads::CDockManager* m_DockManager;

public slots:
    void connect();
    void open_elf();

};





#endif // STPLOTWINDOW_H
