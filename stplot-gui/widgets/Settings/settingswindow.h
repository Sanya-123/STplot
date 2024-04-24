#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QStyleFactory> //QStyleFactory::keys() - get alailebel styles

namespace Ui {
class SettingsWindow;
}
QT_BEGIN_NAMESPACE
class STPlotWindow;
QT_END_NAMESPACE

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    struct SettingsMainWindow{
        QString theme;
    };
    SettingsWindow(STPlotWindow *mainWondow, QWidget *parent = nullptr);
    ~SettingsWindow();
    void setSettings(SettingsMainWindow settings);
    void getSettings(SettingsMainWindow &settings);

signals:
    void applySettings();

private:
    Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
