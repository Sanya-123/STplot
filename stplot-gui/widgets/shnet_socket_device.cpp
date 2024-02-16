#include "shnet_socket_device.h"
#include <QThread>
#include <QTime>
SHnetUDPDevice::SHnetUDPDevice() : configWidget(nullptr)
{

}

SHnetUDPDevice::~SHnetUDPDevice()
{

}

int SHnetUDPDevice::initDevise(QVector<ReadAddres> readSeuqence)
{
    this->readSeuqence = readSeuqence;
    memset(&downlink, 0, sizeof(SHnet_link_t));
    memset(&uplink, 0, sizeof(SHnet_link_t));
    // open socket
    udpSocket = new QUdpSocket(this);
    udpSocket->connectToHost(QHostAddress("0.0.0.0"), 31337);
    if (!udpSocket->waitForConnected(3000)){
        return -1;
    }
    qDebug() << "Socket connected";

    // // make request queue
    // int address_cnt = 0;
    // debug_msg_t req = {
    //     .cmd = DEBUG_READ,
    // };
    // for(int i = 0; i < readSeuqence.size(); i++){
    //     int serial_cnt = 0;
    //     for (int j = 0; j < readSeuqence[i].readSize / 4; j++){
    //         req.read_request.addresses[address_cnt] = readSeuqence[i].addres + (serial_cnt * 4);
    //         address_cnt++;
    //         serial_cnt++;
    //         if (address_cnt == DEBUG_DATA_SIZE_WORDS){
    //             address_cnt = 0;
    //             requestQueue.append(req);
    //             memset(&req, 0, sizeof(debug_msg_t));
    //             req.cmd = DEBUG_READ;
    //         }
    //     }
    // }
    // if (address_cnt){
    //     requestQueue.append(req);
    // }

    uplink.net_id_0 = SHNET_ID_KPP;
    uplink.protocol_id = SHNET_MSG_DEBUG;
    // connect(udpSocket, &QUdpSocket::readyRead,
    //              this, &SHnetUDPDevice::readPendingDatagrams);

    return 0;
}

// void SHnetUDPDevice::readPendingDatagrams()
// {
//     udpSocket->readDatagram((char*)&uplink, sizeof(SHnet_link_t), NULL, NULL);
//     qDebug() << "New message";
// }


void SHnetUDPDevice::stopDev()
{
    //    readSeuqence.clear();
    if(udpSocket){
        udpSocket->close();
        // disconnect(udpSocket, &QUdpSocket::readyRead,
        //            this, &SHnetUDPDevice::readPendingDatagrams);
    }
}

bool SHnetUDPDevice::dataRecieved(){
    if((uplink.msg_id == downlink.msg_id)
    && (uplink.net_id_0 == downlink.net_id_0)
    && (uplink.net_id_1 == downlink.net_id_1)
    && (uplink.net_id_2 == downlink.net_id_2)
    && (uplink.net_id_3 == downlink.net_id_3)
    ){
        debug_msg_t* rx_msg = (debug_msg_t*)&uplink.data;
        debug_msg_t* tx_msg = (debug_msg_t*)&downlink.data;
        if (tx_msg->cmd == rx_msg->cmd){
            return true;
        }
    }
    qDebug() << "No reply match";
    return false;

}

int SHnetUDPDevice::processRequest(debug_msg_t* req){
    int timeoutTime = QTime::currentTime().msecsSinceStartOfDay() + 5000;
    memcpy(&downlink.data, req, SHNET_DATA_SIZE_BYTES);
    downlink.msg_id++;
    udpSocket->flush();
    while (true){
        if (udpSocket->write((char*)&downlink, sizeof(SHnet_link_t)) < 0){
            qDebug() << "Socket write error";
            return -1;
        }
        QThread::msleep(20);
        int read = udpSocket->read((char*)&uplink, sizeof(SHnet_link_t));
        if (read < 0){
            qDebug() << "Socket read error";
            return -1;
        }
        else if(read > 0){

        }
        if (dataRecieved()){
            qDebug() << "Socket recieved data";
            return 0;
        }
        if (QTime::currentTime().msecsSinceStartOfDay() > timeoutTime){
            return -1;
            qDebug() << "Socket read timeout";
        }
    }
    return 0;
}

int SHnetUDPDevice::execReadDevice()
{
    if(udpSocket == NULL){
        return -1;
    }
    debug_msg_t req = {
        .cmd = DEBUG_READ,
    };
    int address_cnt = 0;
    int serial_cnt = 0;
    memset(&downlink.data, 0, SHNET_DATA_SIZE_BYTES);
    QVector<QVector<uint32_t>> emittList;
    for(int i = 0; i < readSeuqence.size(); i++){
        serial_cnt = 0;
        QVector<uint32_t> readData(readSeuqence[i].readSize / 4);
        for (int j = 0; j < readSeuqence[i].readSize / 4; j++){
            req.read_request.addresses[address_cnt] = readSeuqence[i].addres + (serial_cnt * 4);
            address_cnt++;
            serial_cnt++;
            if (address_cnt == DEBUG_DATA_SIZE_WORDS){
                address_cnt = 0;
                // send request and copy data to readData
                int ret = processRequest(&req);
                if (ret == 0){
                    debug_msg_t* response = (debug_msg_t*)&uplink.data;
                    memcpy(readData.data(), response->read_reply.values, DEBUG_DATA_SIZE_WORDS);
                }
                else{
                    // error processing request
                    return -1;
                }
                memset(&req, 0, sizeof(debug_msg_t));
                req.cmd = DEBUG_READ;
            }
        }
        emittList.append(readData);
    }
    if (address_cnt > 0){
        memset(&downlink.data, 0, SHNET_DATA_SIZE_BYTES);
        QVector<uint32_t> &pData = emittList.last();
        int ret = processRequest(&req);
        if (ret == 0){
            debug_msg_t* response = (debug_msg_t*)&uplink.data;
            memcpy(pData.data(), response->read_reply.values, address_cnt);
        }
        else{
            // error processing request
            return -1;
        }
    }
    QDateTime dt = QDateTime::currentDateTime();
    for (int i = 0; i < emittList.size(); i++){
        emit addressesReedWithTime32(readSeuqence[i].addres, emittList.value(i), dt);
    }
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
