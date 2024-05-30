#ifndef STLINKDEVICE_H
#define STLINKDEVICE_H

#include "readwritedevice.h"
#include <QMap>

extern "C" {
#include "stlink.h"
#include <usb.h>
}

class STlinkDevice : public ReadDeviceObject
{
    Q_OBJECT
public:
    STlinkDevice();
    ~STlinkDevice();
    bool isFileDevice() {return false;}
//    int initDevise(QVector<VarChannel*> *channels);
    int initDevise(QVector<struct ReadAddres> readSeuqence);
    void stopDev();
    int execReadDevice();
    int writeDataDevice(uint64_t data, varloc_location_t location);
    QDialog *getReadDevConfigDialog();

private:
//    //addres and waribels with curent addres
//    struct ReadChanale {
//        VarChannel* chanale;
//        uint8_t offset;
//        uint8_t varSize;
//    };
//    struct ReadAddres {
//        QVector<struct ReadChanale> vectorChanales;//varivels with offset for this addres
//        uint32_t addres;//begin addres read
//        uint32_t readSize;//read addresses
//    };
    QVector<struct ReadAddres> readSeuqence;

    //dev
    stlink_t* sl = NULL;

    QDialog *configWidget;
};

#endif // STLINKDEVICE_H
