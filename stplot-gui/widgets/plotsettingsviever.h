#ifndef PLOTSETTINGSVIEVER_H
#define PLOTSETTINGSVIEVER_H

#include <QWidget>
#include "plotinterface.h"
#include "PropertyWidget.h"
#include "PropertyCore.h"
#include <QDialog>

class PlotSettingsViever : public QDialog
{
    Q_OBJECT
public:
    explicit PlotSettingsViever(PlotSettingsAbstract *settings, QWidget *parent = nullptr);
    ~PlotSettingsViever();

private slots:
    void changeValue(QtnPropertyChangeReason reson);

signals:

private:
    QtnPropertySet *propertySets;
    QtnPropertyWidget propWidget;
    PlotSettingsAbstract *settings;
};

#endif // PLOTSETTINGSVIEVER_H
