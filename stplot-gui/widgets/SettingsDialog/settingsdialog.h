#ifndef SettingsDialog_H
#define SettingsDialog_H

#include <QWidget>
#include "settingsabstract.h"
#include "PropertyWidget.h"
#include "PropertyCore.h"
#include <QDialog>
#include <QSettings>

#define VIEW_MODE_FILE_NAME                         "settingsmodels.conf"

QT_BEGIN_NAMESPACE
class QComboBox;
class QLineEdit;
class QPushButton;
QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(SettingsAbstract *settings, QWidget *parent = nullptr);
    ~SettingsDialog();

private:
    QtnPropertySet *findParentProperty(QString propertyName, QString *displayName, QtnPropertySet *propertySets);

private slots:
    void changeValue(QtnPropertyChangeReason reson);
    void saveSettings();
    void loadSettings();

signals:

private:
    QtnPropertySet *propertySets;
    QtnPropertyWidget propWidget;
    SettingsAbstract *settings;

    QComboBox *loadSettingsName;
    QLineEdit *saveSettingsName;
    QPushButton *loadSettingsButton, *saveSettingsButton;
    QSettings settingsTemplates;
    //name for prorerty shoudn't containt '.' so replase it and seve name with replase and without replase
    QMap<QString, QString> mapPpopNameToParName;
};

#endif // SettingsDialog_H
