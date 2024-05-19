#ifndef TELEMETRY_FILE_H
#define TELEMETRY_FILE_H

#include <QObject>
#include "readwritedevice.h"
#include "telemetryfiledevice.h"

class TelemetryFile : public QObject, SaveDeviceInterfacePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.S.debuger.TelemetryFileDevice" FILE "pluginTelemetryFileDevice.json")
    Q_INTERFACES(SaveDeviceInterfacePlugin)

public:
    TelemetryFile(QObject *parent = nullptr) : QObject(parent) {}
//    ReadDeviceObject* createReadDeviceObject() {return &device;}
    SaveDeviceObject* createWriteDeviceObject() {return &device;}
    QString getName() {return "TelemetryFile";}

private:
    TelemetryFileDevice device;
};

#endif // TELEMETRY_FILE_H
