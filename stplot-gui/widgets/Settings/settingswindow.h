#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

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

private:
    Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
