#ifndef SHNET_SOCKET_DEVICE_H
#define SHNET_SOCKET_DEVICE_H

#include "readwritedevice.h"
#include <QMap>
#include <QUdpSocket>

class SHnetUDPDevice : public ReadDeviceObject
{
    Q_OBJECT
public:
    SHnetUDPDevice();
    ~SHnetUDPDevice();
    bool isFileDevice() {return false;}
    int initDevise(QVector<struct ReadAddres> readSeuqence);
    void stopDev();
    int execReadDevice();
    int writeDataDevice(uint32_t data, varloc_location_t location);
    QWidget *getReadDevConfigWidget();

private slots:
    void readPendingDatagrams();

private:
    QVector<struct ReadAddres> readSeuqence;
    QWidget *configWidget;
    QHostAddress serverAddress;
    int serverPort;
    QUdpSocket* udpSocket;
};

#endif // SHNET_SOCKET_DEVICE_H
