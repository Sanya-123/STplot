#include "stlinkdevice.h"

STlinkDevice::STlinkDevice() : configWidget(nullptr)
{

}

//int STlinkDevice::initDevise(QVector<VarChannel *> *channels)
//{
//    if(channels == nullptr)
//        return -1;

//    if(channels->size() < 1)
//        return -1;

//    //NOTE this is very importatn stuff for make sequence for readn with optimized addres reads
//    readSeuqence.clear();
//    QMap<uint32_t, VarChannel *> addresVarMap;
//    //TODO var with same add reses
//    for(int i = 0 ; i < channels->size(); i++)
//    {
//        addresVarMap[channels->at(i)->addres()] = channels->at(i);
//    }

//    QList<uint32_t> addresses = addresVarMap.keys();

//    struct ReadAddres readAdd;
//    struct ReadChanale readChan;
//    readChan.chanale = addresVarMap[addresses[0]];
//    readChan.offset = addresses[0]%4;
//    readChan.varSize = 4;

//    readAdd.vectorChanales.append(readChan);
//    readAdd.addres = (addresses[0]/4)*4;
//    readAdd.readSize = 4;
//    for(int i = 1; addresses.size(); i++)
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
//            readSeuqence.append(readAdd);
//            readAdd.vectorChanales.clear();

//            readAdd.addres = addresVaribel;
//            readAdd.vectorChanales.append(readChan);
//        }
//    }


//    readSeuqence.append(readAdd);


//    return -1;

//    enum connect_type connect = CONNECT_HOT_PLUG;
//    int32_t freq = 100000;
//    sl = stlink_open_usb(UERROR, connect, NULL, freq);
//    if (sl == NULL){
//        return(-1);
//    }

//    stlink_enter_swd_mode(sl);

//    return 0;

//}

int STlinkDevice::initDevise(QVector<ReadAddres> readSeuqence)
{
    this->readSeuqence = readSeuqence;
//    return -1;

    enum connect_type connect = CONNECT_HOT_PLUG;
    int32_t freq = 100000;
    sl = stlink_open_usb(UERROR, connect, NULL, freq);
    if (sl == NULL){
        return(-1);
    }

    stlink_enter_swd_mode(sl);

    return 0;
}

void STlinkDevice::stopDev()
{
//    readSeuqence.clear();
    if (sl)
    {
        stlink_exit_debug_mode(sl);
        stlink_close(sl);
    }
}

int STlinkDevice::execReadDevice()
{
    if(!sl)
        return -1;

    struct {
        float _f;
        uint8_t _8[4];
        uint8_t _16[2];
        uint8_t _32;
    }combiner;

    for(int i = 0; i < readSeuqence.size(); i++)
    {
        int32_t res = stlink_read_mem32(sl, readSeuqence[i].addres, readSeuqence[i].readSize);
        if(res != 0)
            return -1;

        QVector<uint8_t> reedData(readSeuqence[i].readSize);
        memcpy(reedData.data(), sl->q_buf, readSeuqence[i].readSize);
        emit addressesReed(readSeuqence[i].addres, reedData);
    }

    return 0;
}

QWidget *STlinkDevice::getReadDevConfigWidget()
{
    return configWidget;
}
