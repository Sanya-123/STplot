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
    connect(ui->pushButton_apply, SIGNAL(clicked(bool)), this, SIGNAL(applySettings()));
    connect(ui->pushButton_apply, SIGNAL(clicked(bool)), mainWondow, SLOT(applySettingsSlot()));
    connect(this, SIGNAL(accepted()), mainWondow, SLOT(applySettingsSlot()));
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::setSettings(SettingsMainWindow settings)
{
    ui->comboBox_theme->setCurrentText(settings.theme);
    ui->checkBox_autoCleanGruph->setChecked(settings.autoclenaOnStart);
}

void SettingsWindow::getSettings(SettingsMainWindow &settings)
{
    settings.theme = ui->comboBox_theme->currentText();
    settings.autoclenaOnStart = ui->checkBox_autoCleanGruph->isChecked();
}
