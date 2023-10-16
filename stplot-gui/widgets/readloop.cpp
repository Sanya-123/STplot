#include "readloop.h"
#include <QDebug>
#include <QThread>

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

//    readDevicec->moveToThread(this->thread());
//    for(int i = 0; i < saveDeviceces->size(); i++)
//        saveDeviceces->at(i)->moveToThread(this->thread());

    connect(readDevicec, SIGNAL(addressesReed(uint32_t,QVector<uint8_t>)), this, SIGNAL(addressesReed(uint32_t,QVector<uint8_t>)));

    bool isFileDev = readDevicec->isFileDevice();
    try {

        //try init dev
//        int res = readDevicec->initDevise(channels);
        int res = readDevicec->initDevise(readSequence);
        if(res != 0)//error read device
            throw res;

        //main loop of read data
        int tmp_i = 0;//TODO temporary for check withput thread
        do{
            //read value
            int resRead = readDevicec->execReadDevice();
            if(resRead != 0)//error read device
                throw resRead;

            //savde data in
            if(!isFileDev && saveDeviceces != nullptr)
            {
                for(int i = 0; i < saveDeviceces->size(); i++)
                    saveDeviceces->at(i)->execSaveDevice();
            }

            //TODO temporary for check withput thread
            tmp_i++;
            if(tmp_i == 1000)
                stopSignal = true;

            QThread::msleep(2);

        }while(stopSignal == false);

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
    disconnect(readDevicec, SIGNAL(addressesReed(uint32_t,QVector<uint8_t>)), this, SIGNAL(addressesReed(uint32_t,QVector<uint8_t>)));
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

void ReadLoop::setReadSequence(const QVector<ReadDeviceObject::ReadAddres> &newReadSequence)
{
    readSequence = newReadSequence;
}

void ReadLoop::setChannels(QVector<VarChannel *> *newChannels)
{
    channels = newChannels;
}
