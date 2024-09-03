#include "telemetryfiledevice.h"
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QDialogButtonBox>

TelemetryFileDevice::TelemetryFileDevice()
{
    stopDev();
    configReadWidget = new QDialog;
    QLabel *labelFile = new QLabel("File:", configReadWidget);
    folderName = new QLineEdit(configReadWidget);
    QPushButton *buttomSelectFolder = new QPushButton("open folder", configReadWidget);
    connect(buttomSelectFolder, SIGNAL(clicked(bool)), this, SLOT(openSelectFolder()));

    QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, configReadWidget);
    connect(dialogButtonBox, SIGNAL(accepted()), configReadWidget, SLOT(accept()));
    connect(dialogButtonBox, SIGNAL(rejected()), configReadWidget, SLOT(reject()));

    QVBoxLayout* layoutVertical = new QVBoxLayout(configReadWidget);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(labelFile);
    layout->addWidget(folderName);
    layout->addWidget(buttomSelectFolder);

    layoutVertical->addLayout(layout);
    layoutVertical->addWidget(dialogButtonBox);
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

QDialog *TelemetryFileDevice::getReadDevConfigDialog()
{
    return configReadWidget;
}

void TelemetryFileDevice::saveSettings(QSettings *settings)
{
    settings->setValue("readfile", folderName->text());
}

void TelemetryFileDevice::restoreSettings(QSettings *settings)
{
    folderName->setText(settings->value("readfile").toString());
}

#include <QDebug>
int TelemetryFileDevice::readTelemetryDir(QString path, QVector<VarChannel *> chanales, AbstractFileProgress *fileProgress, QVector<QTime> *readTimes){
    QDir dir(path);
    QFileInfoList teleFiles = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoDot);
    fileProgress->setMessadge("Path:" + path);
    float procentPerElement = MAX_PROCENT_FILE_DIALOG/teleFiles.length();
    for (int j = 0; j < teleFiles.length(); j++){
        fileProgress->setProgress(procentPerElement*j);
        QFileInfo file = teleFiles[j];
        if (file.isDir()){
            int ret = readTelemetryDir(file.absoluteFilePath(), chanales, fileProgress, readTimes);
            if (ret != 0){
                return ret;
            }
        }
        if(file.isFile() && file.suffix() == "t"){

            int ret = readTelemetryFile(file.absoluteFilePath(), chanales, readTimes);
            if (ret != 0){
                return  ret;
            }

        }
    }
    return 0;
}

int TelemetryFileDevice::readTelemetryFile(QString filepath, QVector<VarChannel *> chanales, QVector<QTime> *readTimes){
    QFile teleRecord(filepath);
    if (!teleRecord.open(QIODevice::ReadOnly)){
        qWarning("Error opening telemetry record");
        return -3;
    }
    QByteArray pickle = teleRecord.readAll();
    if(pickle.size() < 598)
    {
        qWarning("Error reading telemetry record");
        return -3;
    }

    if (pickle.at(80) != 0x71 || pickle.at(81) != 0x04){

        qWarning("Error parsing telemetry record");
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

    if(readTimes != nullptr)
        readTimes->append(dt.time());

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
                qWarning("Channel not found in telemetry record - %s", ch->getName().toLocal8Bit().data());
                return -3;
            }
        }
    }
    return 0;
}

int TelemetryFileDevice::readFileDevice(QVector<VarChannel *> chanales, AbstractFileProgress *fileProgress, QVector<QTime> *readTimes)
{
    int ret = readTelemetryDir(folderName->text(), chanales, fileProgress, readTimes);
    stopDev();
    return ret;
}

void TelemetryFileDevice::openSelectFolder()
{
    QString folder = QFileDialog::getExistingDirectory(configReadWidget, tr("Select folder"), folderName->text());
    if(!folder.isEmpty())
        folderName->setText(folder);
}

