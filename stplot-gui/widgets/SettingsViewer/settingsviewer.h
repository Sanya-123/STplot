#ifndef SettingsViewer_H
#define SettingsViewer_H

#include <QWidget>
#include "settingsabstract.h"
#include "PropertyWidget.h"
#include "PropertyCore.h"
#include <QDialog>

class SettingsViewer : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsViewer(SettingsAbstract *settings, QWidget *parent = nullptr);
    ~SettingsViewer();

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

#endif // SettingsViewer_H
