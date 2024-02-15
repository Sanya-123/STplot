#include "shnet_socket_device.h"
SHnetUDPDevice::SHnetUDPDevice() : configWidget(nullptr)
{

}

SHnetUDPDevice::~SHnetUDPDevice()
{

}

int SHnetUDPDevice::initDevise(QVector<ReadAddres> readSeuqence)
{
    this->readSeuqence = readSeuqence;

    udpSocket = new QUdpSocket(this);
    bool bound = udpSocket->bind(QHostAddress("192.168.5.1"), 31337);
    if (!bound){
        return -1;
    }

    connect(udpSocket, &QUdpSocket::readyRead,
                 this, &SHnetUDPDevice::readPendingDatagrams);

    return 0;
}

void SHnetUDPDevice::readPendingDatagrams()
{

}


void SHnetUDPDevice::stopDev()
{
    //    readSeuqence.clear();
    if (udpSocket)
    {
        udpSocket->close();
        disconnect(udpSocket, &QUdpSocket::readyRead,
                   this, &SHnetUDPDevice::readPendingDatagrams);
    }
}

int SHnetUDPDevice::execReadDevice()
{
    // if(!sl)
    //     return -1;

    // struct {
    //     float _f;
    //     uint8_t _8[4];
    //     uint8_t _16[2];
    //     uint8_t _32;
    // }combiner;

    // for(int i = 0; i < readSeuqence.size(); i++)
    // {
    //     int32_t res = stlink_read_mem32(sl, readSeuqence[i].addres, readSeuqence[i].readSize);
    //     if(res != 0)
    //         return -1;
    //     QDateTime dt = QDateTime::currentDateTime();
    //     QVector<uint8_t> reedData(readSeuqence[i].readSize);
    //     memcpy(reedData.data(), sl->q_buf, readSeuqence[i].readSize);
    //     // emit addressesReed(readSeuqence[i].addres, reedData);
    //     emit addressesReedWithTime(readSeuqence[i].addres, reedData, dt);
    // }

    return 0;
}

int SHnetUDPDevice::writeDataDevice(uint32_t data, varloc_location_t location)
{
    return 0;
}

QWidget *SHnetUDPDevice::getReadDevConfigWidget()
{
    return configWidget;
}
