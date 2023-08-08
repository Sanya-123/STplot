#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QWidget>

namespace Ui {
class ViewManager;
}

class ViewManager : public QWidget
{
    Q_OBJECT

public:
    explicit ViewManager(QWidget *parent = nullptr);
    ~ViewManager();

private:
    Ui::ViewManager *ui;
};

#endif // VIEWMANAGER_H
