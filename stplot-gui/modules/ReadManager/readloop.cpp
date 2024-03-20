#include "readloop.h"
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>

ReadLoop::ReadLoop(QObject *parent)
    : QObject{parent}, readDevicec(nullptr), saveDeviceces(nullptr), channels(nullptr)
{
}

void ReadLoop::readLoop()
{
    stopSignal = false;
    if(readDevicec == nullptr/* || channels == nullptr*/)
    {
        emit stopedLoop();
        return;
    }

    saveSequence.clear();
    requestedWriteData.clear();

    bool isFileDev = readDevicec->isFileDevice();

//    readDevicec->moveToThread(this->thread());
//    for(int i = 0; i < saveDeviceces->size(); i++)
//        saveDeviceces->at(i)->moveToThread(this->thread());

    connect(readDevicec, SIGNAL(addressesReedWithTime(uint32_t,QVector<uint8_t>,QDateTime)), this, SIGNAL(addressesReedWithTime(uint32_t,QVector<uint8_t>,QDateTime)));
    if(!isFileDev)
        connect(readDevicec, SIGNAL(addressesReedWithTime(uint32_t,QVector<uint8_t>,QDateTime)), this, SLOT(saveReedSequence(uint32_t,QVector<uint8_t>,QDateTime)));

    try {

        //try init dev
//        int res = readDevicec->initDevise(channels);
        int res = readDevicec->initDevise(readSequence);
        if(res != 0)//error read device
            throw res;

        if(!isFileDev && saveDeviceces != nullptr)
        {
            for(int i = 0; i < saveDeviceces->size(); i++)
                saveDeviceces->at(i)->initDevise(readSequence);
        }

//        QElapsedTimer timer;
//        timer.start();

        //main loop of read data
        do{
            //read value
            int resRead = readDevicec->execReadDevice();
            if(resRead != 0)//error read device
                throw resRead;

            //savde data in
            if(!isFileDev && saveDeviceces != nullptr)
            {
                for(int i = 0; i < saveDeviceces->size(); i++)
                    saveDeviceces->at(i)->execSaveDevice(saveSequence);
            }

            saveSequence.clear();

            //write data
            while(requestedWriteData.size())
            {
                readDevicec->writeDataDevice(requestedWriteData[0].first, requestedWriteData[0].second);
                requestedWriteData.removeFirst();
            }
           QThread::msleep(2);

        }while(stopSignal == false);

//        qDebug () << "spendTime:" << timer.elapsed();

    } catch (int err) {
        Q_UNUSED(err);
        qDebug() << err;
    }
    //stop
    //stop read
    readDevicec->stopDev();
    //stao all dev if it nesasary
    if(!isFileDev && saveDeviceces != nullptr)
    {
        for(int i = 0; i < saveDeviceces->size(); i++)
            saveDeviceces->at(i)->stopDev();
    }
     disconnect(readDevicec, SIGNAL(addressesReedWithTime(uint32_t,QVector<uint8_t>,QDateTime)), this, SIGNAL(addressesReedWithTime(uint32_t,QVector<uint8_t>,QDateTime)));
    if(!isFileDev)
        disconnect(readDevicec, SIGNAL(addressesReedWithTime(uint32_t,QVector<uint8_t>,QDateTime)), this, SLOT(saveReedSequence(uint32_t,QVector<uint8_t>,QDateTime)));
    saveSequence.clear();
    emit stopedLoop();
}

void ReadLoop::setSaveDeviceces(QVector<SaveDeviceObject *> *newSaveDeviceces)
{
    saveDeviceces = newSaveDeviceces;
}
void ReadLoop::setReadDevicec(ReadDeviceObject *newReadDevicec)
{
    readDevicec = newReadDevicec;
}

void ReadLoop::stopLoop()
{
    stopSignal = true;
}

void ReadLoop::requestWriteData(uint32_t data, varloc_location_t location)
{
    requestedWriteData.append(qMakePair(data, location));
}

void ReadLoop::saveReedSequence(uint32_t addres, QVector<uint8_t> data, QDateTime time)
{
    Q_UNUSED(time);
    saveSequence.append(qMakePair(addres, data));
}

void ReadLoop::setReadSequence(const QVector<ReadDeviceObject::ReadAddres> &newReadSequence)
{
    readSequence = newReadSequence;
}

void ReadLoop::setChannels(QVector<VarChannel *> *newChannels)
{
    channels = newChannels;
}
