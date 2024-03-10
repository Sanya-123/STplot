#ifndef SettingsDialog_H
#define SettingsDialog_H

#include <QWidget>
#include "settingsabstract.h"
#include "PropertyWidget.h"
#include "PropertyCore.h"
#include <QDialog>

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

signals:

private:
    QtnPropertySet *propertySets;
    QtnPropertyWidget propWidget;
    SettingsAbstract *settings;
};

#endif // SettingsDialog_H
