#ifndef SHNET_SOCKET_H
#define SHNET_SOCKET_H

#include <QObject>
#include "readwritedevice.h"
#include "shnet_socket_device.h"

class SHnetSocket : public QObject, ReadDeviceInterfacePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.S.debuger.SHnetSocketDevice" FILE "pluginSHnetSocketDevice.json")
    Q_INTERFACES(ReadDeviceInterfacePlugin)

public:
    SHnetSocket(QObject *parent = nullptr) : QObject(parent) {}
    ReadDeviceObject* createReadDeviceObject() {return new SHnetUDPDevice;}
    QString getName() {return "SHnetUDP";}

private:

};

#endif // SHNET_SOCKET_H
