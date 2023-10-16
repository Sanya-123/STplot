#include "simplereader.h"

SimpleReader::SimpleReader(QObject *parent)
    : QObject{parent}, stlink(nullptr)
{

}

int SimpleReader::connect(){
    enum connect_type connect = CONNECT_HOT_PLUG;
    int32_t freq = 100000;
    stlink = stlink_open_usb(UERROR, connect, NULL, freq);
    if (stlink == NULL){
        return(-1);
    }

    stlink_enter_swd_mode(stlink);
    return 0;
}

int SimpleReader::readData(){

    QMapIterator<uint32_t, QVector<VarChannel*>*> iter(read_sequence);
    while (iter.hasNext()) {
        iter.next();
        uint32_t address = iter.key();
        int32_t res = stlink_read_mem32(stlink, address, 4);
        if(res != 0){
            return -1;
        }
        uint32_t data;
        memcpy(&data, stlink->q_buf, 4);
        QVector<VarChannel*>* channels = iter.value();
        for (int i = 0; i < channels->size(); ++i) {
            channels->at(i)->pushValueRaw(data);
        }
        return 0;
    }
}

bool SimpleReader::isConnected(){
    return (stlink != nullptr);
}

void SimpleReader::loadChannels(QVector<VarChannel *> *channels){
    for (int i = 0; i < channels->size(); ++i) {
        uint32_t address = channels->at(i)->getLocation().address.base;
        if (read_sequence.contains(address)){
            read_sequence[address]->append(channels->at(i));
        }
        else{
            QVector<VarChannel*> *vector = new QVector<VarChannel*> ;
            vector->append(channels->at(i));
            read_sequence[address] = vector;
        }
    }
}
