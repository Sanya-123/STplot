#include "stlinkdevice.h"

STlinkDevice::STlinkDevice() : configWidget(nullptr)
{

}

int STlinkDevice::initDevise(QVector<VarChannel *> *channels)
{
    //NOTE this is very importatn stuff for make sequence for readn with optimized addres reads
    return -1;

    enum connect_type connect = CONNECT_HOT_PLUG;
    int32_t freq = 100000;
    sl = stlink_open_usb(UERROR, connect, NULL, freq);
    if (sl == NULL){
        return(-1);
    }

    stlink_enter_swd_mode(sl);

}

void STlinkDevice::stopDev()
{
    readSeuqence.clear();
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

        for(int j = 0 ; j < readSeuqence[i].vectorChanales.size(); j++)
        {
            combiner._32 = 0;
            memcpy(combiner._8, sl->q_buf + readSeuqence[i].vectorChanales[j].offset, readSeuqence[i].vectorChanales[j].varSize);

            //TODO some logig base on type
            readSeuqence[i].vectorChanales[j].chanale->push_value(combiner._32*1.0);
        }
    }

    return 0;
}

QWidget *STlinkDevice::getReadDevConfigWidget()
{
    return configWidget;
}
