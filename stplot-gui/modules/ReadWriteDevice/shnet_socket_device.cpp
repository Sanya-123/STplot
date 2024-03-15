#include "shnet_socket_device.h"
#include <QThread>
#include <QTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QSpacerItem>

SHnetUDPDevice::SHnetUDPDevice()
{
    serverAddress = "127.0.0.1";
    serverPort = 31337;

    shnetIDMap.insert("KPP", SHNET_ID_KPP);
    shnetIDMap.insert("KLCD", SHNET_ID_KLCD);
    shnetIDMap.insert("KBO_TRACK", SHNET_ID_KBO_TRACK);
    shnetIDMap.insert("KBO_LIFT", SHNET_ID_KBO_LIFT);
    // shnetIDMap.insert("KPP", SHNET_ID_KUE_TRACK);
    // shnetIDMap.insert("KPP", SHNET_ID_KUE_LIFT);
    shnetIDMap.insert("IMU_BODY", SHNET_ID_INCL_BODY);
    shnetIDMap.insert("IMU_PLATFORM", SHNET_ID_INCL_PLATFORM);
    shnetIDMap.insert("NAV", SHNET_ID_NAV);
    shnetIDMap.insert("PU", SHNET_ID_PU);
    shnetIDMap.insert("HEC_FRONT", SHNET_ID_HEC_FRONT);
    shnetIDMap.insert("HEC_REAR", SHNET_ID_HEC_REAR);
    shnetIDMap.insert("HEC_RIGHT", SHNET_ID_SHELF_RIGHT);
    shnetIDMap.insert("HEC_LEFT", SHNET_ID_SHELF_LEFT);
    shnetIDMap.insert("HEC_TOP", SHNET_ID_SHELF_TOP);
    shnetIDMap.insert("RADAR_FRONT", SHNET_ID_RADAR_FRONT);
    shnetIDMap.insert("RADAR_REAR", SHNET_ID_RADAR_REAR);
    shnetIDMap.insert("RADAR_SIDE", SHNET_ID_RADAR_SIDE);
    downlink.net_id_0 = SHNET_ID_KPP;

}

SHnetUDPDevice::~SHnetUDPDevice()
{
}

int SHnetUDPDevice::initDevise(QVector<ReadAddres> readSeuqence)
{
    qDebug() << "Init SHnet UDP socket device";
    this->readSeuqence = readSeuqence;
    memset(&downlink, 0, sizeof(SHnet_link_t));
    memset(&uplink, 0, sizeof(SHnet_link_t));
    addresses.clear();
    data.clear();
    // fill addresses
    for (int i = 0; i < readSeuqence.size(); i++){
        // qDebug("Read sequence #%d %x %d", i, readSeuqence[i].addres, readSeuqence[i].readSize);
        for (int j = 0; j < readSeuqence[i].readSize / 4; j++){
            addresses.append(readSeuqence[i].addres + (j*4));
            data.append(0);
            // qDebug("Address %d %x", j, readSeuqence[i].addres + (j*4));
        }
    }
    qDebug("Sequence size is %d addresses, %f debug frames",
           addresses.size(),(float)addresses.size()/DEBUG_DATA_SIZE_WORDS);

    // open socket
    udpSocket = new QUdpSocket(this);
    udpSocket->connectToHost(QHostAddress(serverAddress), serverPort);
    if (!udpSocket->waitForConnected(5000)){
        return -1;
    }
    qDebug() << "Socket connected";

    downlink.protocol_id = SHNET_MSG_DEBUG;
    qDebug() << downlink.msg_id;

    return 0;
}




void SHnetUDPDevice::stopDev()
{
    if(udpSocket){
        udpSocket->close();
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
        QThread::msleep(100);
        int read = udpSocket->read((char*)&uplink, sizeof(SHnet_link_t));
        if (read < 0){
            qDebug() << "Socket read error";
            return -1;
        }
        else if(read > 0){

        }
        if (dataRecieved()){
            // qDebug() << "Socket recieved data";
            return 0;
        }
        if (QTime::currentTime().msecsSinceStartOfDay() > timeoutTime){
            return -1;
            qDebug() << "Socket read timeout";
        }
    }
    return -1;
}

int SHnetUDPDevice::execReadDevice()
{
    if(udpSocket == NULL){
        return -1;
    }
    debug_msg_t req = {
        .cmd = DEBUG_READ,
    };
    memset(&downlink.data, 0, SHNET_DATA_SIZE_BYTES);
    memset(&uplink.data, 0, SHNET_DATA_SIZE_BYTES);

    int address_cnt = 0;
    int data_cnt = 0;
    int remaining = addresses.size() % DEBUG_DATA_SIZE_WORDS;
    int frames = addresses.size() / DEBUG_DATA_SIZE_WORDS;
    // collect addresses into debug frames
    for (int adr = 0; adr < addresses.size(); adr++){
        req.read_request.addresses[address_cnt] = addresses[adr];
        address_cnt++;
        if (address_cnt == DEBUG_DATA_SIZE_WORDS){
            // have full frame
            int ret = processRequest(&req);
                if (ret == 0){
                    // copy data
                    debug_msg_t* response = (debug_msg_t*)&uplink.data;
                    for (int i = 0; i < DEBUG_DATA_SIZE_WORDS; i++){
                        data[data_cnt] = response->read_reply.values[i];
                        data_cnt++;
                    }
                    memset(&req.read_request, 0, DEBUG_DATA_SIZE_WORDS * 4);
                }
            else{
                // error processing request
                return -1;
            }
        }
    }
    if (remaining > 0){
        // last not full debug frame
        for (int i = 0; i < remaining; i++){
            req.read_request.addresses[i] = addresses[(frames * DEBUG_DATA_SIZE_WORDS) + i];
        }
        int ret = processRequest(&req);
        if (ret == 0){
            debug_msg_t* response = (debug_msg_t*)&uplink.data;
            for (int i = 0; i < remaining; i++){
                data[(frames * DEBUG_DATA_SIZE_WORDS) + i] = response->read_reply.values[i];
            }
            memset(&req.read_request, 0, DEBUG_DATA_SIZE_WORDS * 4);
        }
        else{
            // error processing request
            return -1;
        }
    }

    // emit collected data
    int packed = 0;
    QDateTime dt = QDateTime::currentDateTime();
    for (int i = 0; i < readSeuqence.size(); i++){
        QVector<uint8_t> ret (readSeuqence[i].readSize);
        memcpy(ret.data(), &data[packed], ret.size());
        packed += ret.size() / 4;
        emit addressesReedWithTime(readSeuqence[i].addres, ret, dt);
    }
    return 0;

}

int SHnetUDPDevice::writeDataDevice(uint32_t data, varloc_location_t location)
{
    return 0;
}

void SHnetUDPDevice::setServerAddress(const QString &text)
{
    serverAddress = text;
}

void SHnetUDPDevice::setServerPort(int port)
{
    serverPort = port;
}

void SHnetUDPDevice::setSHnetL0Address(const QString &text)
{
    downlink.net_id_0 = shnetIDMap.value(text);
    qDebug("Set SHnetID: %d",downlink.net_id_0);
}
void SHnetUDPDevice::setSHnetL1Address(int adr)
{
    downlink.net_id_1 = adr;
}
void SHnetUDPDevice::setSHnetL2Address(int adr)
{
    downlink.net_id_2 = adr;
}
void SHnetUDPDevice::setSHnetL3Address(int adr)
{
    downlink.net_id_3 = adr;
}

QWidget *SHnetUDPDevice::getReadDevConfigWidget()
{

    QWidget* configWidget = new QWidget();
    QLabel *labelIP = new QLabel("IP:");
    QLabel *labelPort = new QLabel("Port:");
    QLabel *labelShnet = new QLabel("SHnet:");

    labelIP->setAlignment(Qt::AlignRight);
    labelPort->setAlignment(Qt::AlignRight);
    labelShnet->setAlignment(Qt::AlignRight);


    QSpacerItem *spacer = new QSpacerItem(50,0);

    QLineEdit* serverAddressWidget = new QLineEdit();
    serverAddressWidget->setText(serverAddress);
    serverAddressWidget->setMaxLength(15);
    serverAddressWidget->setMaximumWidth(100);
    // serverAddressWidget->setInputMask("000.000.000.000;_");
    connect(serverAddressWidget, &QLineEdit::textChanged, this, &SHnetUDPDevice::setServerAddress);

    QSpinBox* serverPortWidget = new QSpinBox();
    serverPortWidget->setMinimum(0);
    serverPortWidget->setMaximum(99999);
    serverPortWidget->setMaximumWidth(80);
    serverPortWidget->setValue(serverPort);
    // connect(serverPortWidget, &QSpinBox::valueChanged, this, &SHnetUDPDevice::setServerPort);
    connect(serverPortWidget, SIGNAL(valueChanged(int)), this, SLOT(setServerPort(int)));

    QComboBox *shnetID0 = new QComboBox;
    shnetID0->addItems(QStringList(shnetIDMap.keys()));
    shnetID0->setMaximumWidth(150);
    shnetID0->setCurrentText("KPP");
    connect(shnetID0, &QComboBox::currentTextChanged, this, &SHnetUDPDevice::setSHnetL0Address);

    QSpinBox* shnetID1 = new QSpinBox();
    shnetID1->setMinimum(0);
    shnetID1->setMaximum(31);
    shnetID1->setMaximumWidth(50);
    shnetID1->setValue(downlink.net_id_1);
    connect(shnetID1, SIGNAL(valueChanged(int)), this, SLOT(setSHnetL1Address(int)));

    QSpinBox* shnetID2 = new QSpinBox();
    shnetID2->setMinimum(0);
    shnetID2->setMaximum(31);
    shnetID2->setMaximumWidth(50);
    shnetID2->setValue(downlink.net_id_2);
    connect(shnetID2, SIGNAL(valueChanged(int)), this, SLOT(setSHnetL2Address(int)));

    QSpinBox* shnetID3 = new QSpinBox();
    shnetID3->setMinimum(0);
    shnetID3->setMaximum(7);
    shnetID3->setMaximumWidth(50);
    shnetID3->setValue(downlink.net_id_3);
    connect(shnetID3, SIGNAL(valueChanged(int)), this, SLOT(setSHnetL3Address(int)));


    QHBoxLayout* layout = new QHBoxLayout(configWidget);
    layout->addItem(spacer);
    layout->addWidget(labelIP);
    layout->addWidget(serverAddressWidget);
    layout->addWidget(labelPort);
    layout->addWidget(serverPortWidget);
    layout->addItem(spacer);
    layout->addWidget(labelShnet);
    layout->addWidget(shnetID0);
    layout->addWidget(shnetID1);
    layout->addWidget(shnetID2);
    layout->addWidget(shnetID3);

    return configWidget;
}
