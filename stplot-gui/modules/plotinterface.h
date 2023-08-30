// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef PLOT_WIDGET_INTERFACE_H
#define PLOT_WIDGET_INTERFACE_H

#include <QObject>
#include <QString>
#include <QWidget>
#include <QSettings>
#include <QMap>
#include <QtPlugin>
#include <chrono>
#include "varchannel.h"

#define PLOT_INTERFACE_HEADER_VERSION           0x00000000

union __attribute__((packed)) VersionUnion {
    uint32_t version32;
    struct __attribute__((packed)){
        uint16_t build;
        uint8_t minor;
        uint8_t major;
    }versionS;
};

static QString versionUnionToString(VersionUnion version)
{
    return QString::asprintf("%d.%d.%d", version.versionS.major, version.versionS.minor, version.versionS.build);
}

static QString versionUnionToString(uint32_t version)
{
    VersionUnion _version;
    _version.version32 = version;
    return QString::asprintf("%d.%d.%d", _version.versionS.major, _version.versionS.minor, _version.versionS.build);
}

/**
 * @brief The PlotWidgetAbstract class - abstrac widget clas for plot data
 */
class PlotWidgetAbstract : public QWidget
{
    Q_OBJECT
public:
    PlotWidgetAbstract(QWidget *parent = nullptr) :QWidget(parent) {}
    /**
     * @brief addPlot - add new plot(graph)
     * @param plotName - plot name
     */
    virtual void addPlot(VarChannel *varChanale) = 0;

    /**
     * @brief deletePlot - delete plot
     * @param plotName - plot name
     */
    virtual void deletePlot(VarChannel *varChanale) = 0;

//    /**
//     * @brief plotVar - update plot(update graph)
//     * @param plotName - plot name
//     * @param values - vector of values
//     * @return - trye if plot is created
//     */
//    virtual bool plotVar(QString plotName, QVector<VarValue> values) = 0;

//    virtual void plotVars(QMap<QString, QVector<VarValue>> plotsMap)
//    {
//        for(QMap<QString, QVector<VarValue>>::const_iterator i = plotsMap.constBegin();
//            i != plotsMap.constEnd(); i++)
//        {
//            plotVar(i.key(), i.value());
//        }
//    }

//    virtual void setName(QString name) = 0;
//    virtual QString getName() = 0;

};

/**
 * @brief The PlotWidgetInterfacePlugin class - external plugin with widgets
 */
class PlotWidgetInterfacePlugin
{
public:

    /**
     * @brief getVersion - get version builded library(SHOULDN'T be implemented)
     * @return - ersion builded library
     */
    virtual uint32_t getVersion() {return PLOT_INTERFACE_HEADER_VERSION;}
    /**
     * @brief getWidgets - make new plot widget
     * @return - plot widget (alocate new QWidget)
     */
    virtual PlotWidgetAbstract* createWidgetPlot(QWidget *parent = nullptr) = 0;

    /**
     * @brief getName - get plugin name
     * @return - plugin name
     */
    virtual QString getName() { return QString(""); }

    /**
     * @brief getInfo - get plugin info
     * @return - plugin info
     */
    virtual QString getInfo() { return getName() + ":" + versionUnionToString(getVersion()); }
};

// registrate plugin
QT_BEGIN_NAMESPACE
#define PlotWidgetInterfacePlugin_iid "org.qt-project.S.debuger.PlotWidgetInterfacePlugin"
Q_DECLARE_INTERFACE(PlotWidgetInterfacePlugin, PlotWidgetInterfacePlugin_iid)
QT_END_NAMESPACE


#endif //PLOT_WIDGET_INTERFACE_H
