#include "telemetryfiledevice.h"
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QPushButton>
#include <QFileDialog>

TelemetryFileDevice::TelemetryFileDevice()
{
    stopDev();
    configReadWidget = new QWidget;
    QLabel *labelFile = new QLabel("File:", configReadWidget);
    folderName = new QLineEdit(configReadWidget);
    QPushButton *buttomSelectFolder = new QPushButton("open folder", configReadWidget);
    connect(buttomSelectFolder, SIGNAL(clicked(bool)), this, SLOT(openSelectFolder()));


    QHBoxLayout* layout = new QHBoxLayout(configReadWidget);
    layout->addWidget(labelFile);
    layout->addWidget(folderName);
    layout->addWidget(buttomSelectFolder);
}

TelemetryFileDevice::~TelemetryFileDevice()
{
    delete configReadWidget;
}

bool TelemetryFileDevice::isFileDevice()
{
    return true;
}

int TelemetryFileDevice::initDevise(QVector<struct ReadDeviceObject::ReadAddres> readSeuqence)
{
    locations.clear();
    // isReadMode = false;
    // isWriteMode = false;
    // startTime = QDateTime::currentDateTime();
    this->readSeuqence = readSeuqence;
//    device.setFileName("test.txt");
//    device.setFileName("log_" + startTime.toString("yyyy_MM_dd__hh_mm_ss") +  ".txt");
//    if(!device.open(QIODevice::ReadWrite))
//        return -1;


    return 0;
}

void TelemetryFileDevice::stopDev()
{
    // isReadMode = false;
    // isWriteMode = false;
    // device.close();
}


int TelemetryFileDevice::execReadDevice()
{
    return -1;
}
int TelemetryFileDevice::execSaveDevice(QVector<QPair<uint32_t,QVector<uint8_t>>> saveSequence, QDateTime time)
{
    return -1;
}
int TelemetryFileDevice::execSaveDevice(QList<QString> chanaleNames, QVector<float> listDecoded, QDateTime time)
{
    return -1;
}

QWidget *TelemetryFileDevice::getReadDevConfigWidget()
{
    return configReadWidget;
}

QWidget *TelemetryFileDevice::getSaveDevConfigWidget()
{
    return configSaveWidget;
}

#include <QDebug>
int TelemetryFileDevice::readTelemetryDir(QString path, QVector<VarChannel *> chanales){
    QDir dir(path);
    QFileInfoList teleFiles = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoDot);
    for (int j = 0; j < teleFiles.length(); j++){
        QFileInfo file = teleFiles[j];
        if (file.isDir()){
            int ret = readTelemetryDir(file.absoluteFilePath(), chanales);
            if (ret != 0){
                return ret;
            }
        }
        if(file.isFile() && file.suffix() == "t"){

            int ret = readTelemetryFile(file.absoluteFilePath(), chanales);
            if (ret != 0){
                return  ret;
            }

        }
    }
    return 0;
}

int TelemetryFileDevice::readTelemetryFile(QString filepath, QVector<VarChannel *> chanales){
    QFile teleRecord(filepath);
    if (!teleRecord.open(QIODevice::ReadOnly)){
        qDebug("Error opening telemetry record");
        return -3;
    }
    QByteArray pickle = teleRecord.readAll();
    if (pickle.at(80) != 0x71 || pickle.at(81) != 0x04){

        qDebug("Error parsing telemetry record");
        return -3;
    }

    union lbc {
        char buf[8];
        double  f;
    };
    char* time_ptr = &(pickle.data()[63]);
    union lbc conv;
    conv.buf[0] = time_ptr[7];
    conv.buf[1] = time_ptr[6];
    conv.buf[2] = time_ptr[5];
    conv.buf[3] = time_ptr[4];
    conv.buf[4] = time_ptr[3];
    conv.buf[5] = time_ptr[2];
    conv.buf[6] = time_ptr[1];
    conv.buf[7] = time_ptr[0];
    double time = conv.f;

    int* size = (int*)&(pickle.data()[83]);
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(time*1000);
    // qDebug("Tele data found. Size is %d. Time is %f\n", *size, time);

    // int offset_bits = 0;
    uint32_t* ptr = (uint32_t*)&(pickle.data()[87]);
    for(int i = 0; i < chanales.size(); i++)
    {
        VarChannel* ch = chanales[i];
        if(ch != nullptr){
            varloc_location_t loc = ch->getLocation();
            int word_n = (loc.address.base/* - origin*/) / 4;
            if (word_n < (*size / 4)){
                uint32_t readData = ptr[word_n];
                ch->pushValueRawWithTime(readData, dt);
            }
            else{
                qDebug("Channel not found in telemetry record - %s", ch->getName().toLocal8Bit().data());
                return -3;
            }
        }
    }
    return 0;
}

int TelemetryFileDevice::readFileDevice(QVector<VarChannel *> chanales, QVector<QTime> *readTimes)
{
    int ret = readTelemetryDir(folderName->text(), chanales);
    stopDev();
    return ret;
}

void TelemetryFileDevice::openSelectFolder()
{
    QString folder = QFileDialog::getExistingDirectory(configReadWidget, tr("Select folder"), folderName->text());
    if(!folder.isEmpty())
        folderName->setText(folder);
}

