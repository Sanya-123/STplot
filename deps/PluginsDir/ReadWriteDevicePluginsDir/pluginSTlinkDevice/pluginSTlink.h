#ifndef STLINKD_DEV_H
#define STLINKD_DEV_H

#include <QObject>
#include "readwritedevice.h"
#include "stlinkdevice.h"

class STlink : public QObject, ReadDeviceInterfacePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.S.debuger.STlinkDevice" FILE "pluginSTlinkDevice.json")
    Q_INTERFACES(ReadDeviceInterfacePlugin)

public:
    STlink(QObject *parent = nullptr) : QObject(parent) {}
    ReadDeviceObject* createReadDeviceObject() {return new STlinkDevice;}
    QString getName() {return "STLink";}

private:

};

#endif // STLINKD_DEV_H
