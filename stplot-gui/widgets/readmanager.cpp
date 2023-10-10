#include "readmanager.h"

ReadManager::ReadManager(QObject *parent)
    : QObject{parent}
{

}

int ReadManager::runReadLoop(QVector<VarChannel *> *channels)
{
    this->channels = channels;
    readSeuqencs = calcReadSeuqence(channels);
    readSeuqencsMap.clear();

    //init map read seuuence
    for(int i = 0 ; i < readSeuqencs.size(); i++)
    {
        readSeuqencsMap[readSeuqencs[i].addres] = readSeuqencs[i];
    }

    loop.setReadSequence(readSeuqencs);

    return -1;
}

QVector<ReadDeviceObject::ReadAddres> ReadManager::calcReadSeuqence(QVector<VarChannel *> *channels)
{
    QVector<ReadDeviceObject::ReadAddres> res;

    if(channels == nullptr)
        return res;

    if(channels->size() < 1)
        return res;

    //NOTE this is very importatn stuff for make sequence for readn with optimized addres reads
    QMap<uint32_t, VarChannel *> addresVarMap;
    //TODO var with same add reses
    for(int i = 0 ; i < channels->size(); i++)
    {
        addresVarMap[channels->at(i)->getLocation().address.base] = channels->at(i);
    }

    QList<uint32_t> addresses = addresVarMap.keys();

    struct ReadDeviceObject::ReadAddres readAdd;
    struct ReadDeviceObject::ReadChanale readChan;
    readChan.chanale = addresVarMap[addresses[0]];
    readChan.offset = addresses[0]%4;
    readChan.varSize = 4;

    readAdd.vectorChanales.append(readChan);
    readAdd.addres = (addresses[0]/4)*4;
    readAdd.readSize = 4;
    for(int i = 1; addresses.size(); i++)
    {
        readChan.chanale = addresVarMap[addresses[i]];
        readChan.offset = addresses[i]%4;
        readChan.varSize = 4;


        uint32_t addresVaribel = (addresses[i]/4)*4;
        if(addresVaribel == readAdd.addres)
        {
//            readChan.chanale = addresVarMap[addresses[i]];
//            readChan.offset = addresses[i]%4;
//            readChan.varSize = 4;
            readAdd.vectorChanales.append(readChan);
        }
        else
        {
            res.append(readAdd);
            readAdd.vectorChanales.clear();

            readAdd.addres = addresVaribel;
            readAdd.vectorChanales.append(readChan);
        }
    }


    res.append(readAdd);

    return res;

}

void ReadManager::addresRead(uint32_t addres, QVector<uint8_t> data)
{
    struct {
        float _f;
        uint8_t _8[4];
        uint8_t _16[2];
        uint8_t _32;
    }combiner;

    ReadDeviceObject::ReadAddres addresSequence = readSeuqencsMap[addres];

    for(int j = 0 ; j < addresSequence.vectorChanales.size(); j++)
    {
        combiner._32 = 0;
        memcpy(combiner._8, data.data() + addresSequence.vectorChanales[j].offset, addresSequence.vectorChanales[j].varSize);

        //TODO some logig base on type
        addresSequence.vectorChanales[j].chanale->pushValue(combiner._32*1.0);
    }
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
