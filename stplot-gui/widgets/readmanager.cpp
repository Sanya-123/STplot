#include "readmanager.h"
#include <QDebug>
//#include <QApplication>

ReadManager::ReadManager(QObject *parent)
    : QObject{parent}, readDevicece(nullptr)
{
    loop = new ReadLoop;
    loop->moveToThread(&readLoopThread);
    connect(loop, SIGNAL(addressesReed(uint32_t,QVector<uint8_t>)), this, SLOT(addresRead(uint32_t,QVector<uint8_t>)), Qt::QueuedConnection);
    connect(loop, SIGNAL(stopedLoop()), this, SLOT(stopReadLoop()), Qt::QueuedConnection);
    connect(&readLoopThread, SIGNAL(started()), loop, SLOT(readLoop()));
}

int ReadManager::runReadLoop(QVector<VarChannel *> *channels)
{
    if(readDevicece == nullptr)
        return -1;

    this->channels = channels;
    readSeuqencs = calcReadSeuqence(channels);
//    qDebug() << "readSeuqencs size :" << readSeuqencs.size();
    readSeuqencsMap.clear();

    //init map read seuuence
    for(int i = 0 ; i < readSeuqencs.size(); i++)
    {
//        qDebug("%d:0x%08X;;%d", i, readSeuqencs[i].addres, readSeuqencs[i].readSize);
        readSeuqencsMap[readSeuqencs[i].addres] = readSeuqencs[i];
    }


    loop->setReadSequence(readSeuqencs);
    loop->setReadDevicec(readDevicece);
    loop->setSaveDeviceces(&saveDeviceces);

//    loop->readLoop();

    readDevicece->moveToThread(&readLoopThread);
    for(int i = 0; i < saveDeviceces.size(); i++)
        saveDeviceces[i]->moveToThread(&readLoopThread);

    readLoopThread.start();

    return 0;
}

QVector<ReadDeviceObject::ReadAddres> ReadManager::calcReadSeuqence(QVector<VarChannel *> *channels)
{
    QVector<ReadDeviceObject::ReadAddres> res;

    if(channels == nullptr)
        return res;

    if(channels->size() < 1)
        return res;

    QVector<VarChannel *> sortChanales;
    QVector<VarChannel *> _sortChanales = *channels;

    //sort chanales by addresses
    while(_sortChanales.size())
    {
        VarChannel * lowestChanale = _sortChanales[0];
        int lowestChanaleindex = 0;
        for(int i = 1; i < _sortChanales.size(); i++)
        {
            if(lowestChanale->getLocation().address.base > _sortChanales[i]->getLocation().address.base)
            {
                lowestChanale = _sortChanales[i];
                lowestChanaleindex = i;
            }
        }

        sortChanales.append(lowestChanale);
        _sortChanales.remove(lowestChanaleindex);
    }

    struct ReadDeviceObject::ReadAddres readAdd;
    struct ReadDeviceObject::ReadChanale readChan;
    uint32_t addres = sortChanales[0]->getLocation().address.base;
    readChan.chanale = sortChanales[0];
    readChan.offset = 0;
    readAdd.addres = addres;
    readAdd.readSize = 4;
    readAdd.vectorChanales.append(readChan);

    for(int i = 1; i < sortChanales.size(); i++)
    {
//        qDebug() << "i:" << i;
        if(sortChanales[i]->getLocation().address.base == addres)
        {
//            qDebug("addres is the same:0x%08X", addres);
            readChan.chanale = sortChanales[i];
            readChan.offset = readAdd.readSize - 4;
            readAdd.vectorChanales.append(readChan);
        }
        else if(sortChanales[i]->getLocation().address.base == (addres + 4))
        {
//            qDebug("next addres :0x%08X", addres + 4);
            addres += 4;
            readChan.chanale = sortChanales[i];
            readChan.offset = readAdd.readSize;
            readAdd.readSize += 4;
            readAdd.vectorChanales.append(readChan);
        }
        else
        {
//            qDebug("new addres :0x%08X", sortChanales[i]->getLocation().address.base);
            res.append(readAdd);
            readAdd.vectorChanales.clear();

            readChan.chanale = sortChanales[i];
            readChan.offset = 0;
            addres = sortChanales[i]->getLocation().address.base;
            readAdd.addres = addres;
            readAdd.readSize = 4;
            readAdd.vectorChanales.append(readChan);

        }
    }


    res.append(readAdd);

    return res;


//    //NOTE this is very importatn stuff for make sequence for readn with optimized addres reads
//    QMap<uint32_t, VarChannel *> addresVarMap;
//    //TODO var with same add reses
//    for(int i = 0 ; i < channels->size(); i++)
//    {
//        qDebug("addres:0x%08X;0x%08X", channels->at(i)->getLocation().address.base, channels->at(i)->getMask());
//        addresVarMap[channels->at(i)->getLocation().address.base] = channels->at(i);
//    }

//    QList<uint32_t> addresses = addresVarMap.keys();

//    qDebug() << addresses;

//    return QVector<ReadDeviceObject::ReadAddres>();

//    struct ReadDeviceObject::ReadAddres readAdd;
//    struct ReadDeviceObject::ReadChanale readChan;
//    readChan.chanale = addresVarMap[addresses[0]];
//    readChan.offset = addresses[0]%4;
//    readChan.varSize = 4;

//    readAdd.vectorChanales.append(readChan);
//    readAdd.addres = (addresses[0]/4)*4;
//    readAdd.readSize = 4;
//    for(int i = 1; i < addresses.size(); i++)
//    {
//        readChan.chanale = addresVarMap[addresses[i]];
//        readChan.offset = addresses[i]%4;
//        readChan.varSize = 4;


//        uint32_t addresVaribel = (addresses[i]/4)*4;
//        if(addresVaribel == readAdd.addres)
//        {
////            readChan.chanale = addresVarMap[addresses[i]];
////            readChan.offset = addresses[i]%4;
////            readChan.varSize = 4;
//            readAdd.vectorChanales.append(readChan);
//        }
//        else
//        {
//            res.append(readAdd);
//            readAdd.vectorChanales.clear();

//            readAdd.addres = addresVaribel;
//            readAdd.vectorChanales.append(readChan);
//        }
//    }


//    res.append(readAdd);

//    return res;

}

void ReadManager::addresRead(uint32_t addres, QVector<uint8_t> data)
{
    union __attribute__((packed)){
        float _f;
        uint8_t _8[4];
        uint16_t _16[2];
        uint32_t _32;
    }combiner;

    ReadDeviceObject::ReadAddres addresSequence = readSeuqencsMap[addres];

    for(int j = 0 ; j < addresSequence.vectorChanales.size(); j++)
    {
        combiner._32 = 0;
        memcpy(combiner._8, data.data() + addresSequence.vectorChanales[j].offset, /*addresSequence.vectorChanales[j].varSize*/4);

        //TODO some logig base on type
//        addresSequence.vectorChanales[j].chanale->pushValue(combiner._32*1.0);
        addresSequence.vectorChanales[j].chanale->pushValueRaw(combiner._32);
    }
}

void ReadManager::stopReadLoop()
{
    qDebug() << "stopReadLoop()";
//    return ;
    readLoopThread.quit();
    readLoopThread.wait();
//    readDevicece->moveToThread(QApplication::instance()->thread());
//    readDevicece->moveToThread(this->thread());
//    readDevicece->moveToThread(qApp->thread());
    //BUG
    readDevicece->moveToThread(nullptr);
    for(int i = 0; i < saveDeviceces.size(); i++)
        saveDeviceces[i]->moveToThread(this->thread());
}

void ReadManager::setReadDevicece(ReadDeviceObject *newReadDevicece)
{
    readDevicece = newReadDevicece;
}

//void ReadManager::startRead()
//{
//    //TODO total remeke work with thread
//    ReadDeviceObject* readObj = readDeviceces[0];

//    loop.setChannels(channelsView->getListChanales());
//    loop.setReadDevicec(readObj);
//    loop.setSaveDeviceces(&saveDeviceces);

////    //move all object to thread
////    readObj->moveToThread(&readLoopThread);
////    for(int i = 0; i < saveDeviceces.size(); i++)
////        saveDeviceces[i]->moveToThread(&readLoopThread);
////    readLoop.moveToThread(&readLoopThread);
//    //TODO check and reimplement read this stuff in thread mode


//    loop.readLoop();

//}
