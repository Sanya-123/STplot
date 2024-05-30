#ifndef SHNET_SOCKET_DEVICE_H
#define SHNET_SOCKET_DEVICE_H

#include "readwritedevice.h"
#include <QMap>
#include <QUdpSocket>
#include "PEK_debug.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QSpinBox;
class QLineEdit;
QT_END_NAMESPACE

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
    int writeDataDevice(uint64_t data, varloc_location_t location);
    QDialog *getReadDevConfigDialog();
    void saveSettings(QSettings *settings);
    void restoreSettings(QSettings *settings);

// private slots:
    // void readPendingDatagrams();
private slots:
    void setServerAddress(const QString &text);
    void setServerPort(int port);
    void setSHnetL0Address(const QString &text);
    void setSHnetL1Address(int adr);
    void setSHnetL2Address(int adr);
    void setSHnetL3Address(int adr);

private:
    int processRequest(debug_msg_t* req);
    bool dataRecieved();
    void initConfigWidget();

private:
    QVector<struct ReadAddres> readSeuqence;
    // QVector<debug_msg_t> requestQueue;
    // QHostAddress serverAddress;
    // int serverPort;
    QUdpSocket* udpSocket;
    SHnet_link_t uplink;
    SHnet_link_t downlink;
    int currentRequest;
    QString serverAddress;
    int serverPort;
    QMap<QString, SHnet_ID_L0> shnetIDMap;
    QVector<uint32_t> addresses;
    QVector<uint32_t> data;
    QDialog *configWidget;
    QComboBox *shnetID0;
    QSpinBox* shnetID1, *shnetID2, *shnetID3;
    QLineEdit* serverAddressWidget;
    QSpinBox* serverPortWidget;

};

#endif // SHNET_SOCKET_DEVICE_H
