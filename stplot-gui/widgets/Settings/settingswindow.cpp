#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "stplotwindow.h"

SettingsWindow::SettingsWindow(STPlotWindow *mainWondow, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    ui->comboBox_theme->addItem("none");
    ui->comboBox_theme->addItems(mainWondow->advancedStylesheet.themes());
//    qDebug() << mainWondow->advancedStylesheet.themes();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::setSettings(SettingsMainWindow settings)
{
    ui->comboBox_theme->setCurrentText(settings.theme);
}

void SettingsWindow::getSettings(SettingsMainWindow &settings)
{
    settings.theme = ui->comboBox_theme->currentText();
}
