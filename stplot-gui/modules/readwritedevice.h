#ifndef READDEVICE_H
#define READDEVICE_H

#include <QObject>
#include "varchannel.h"
#include <QPair>

#define READ_WRITE_DEVICE_INTERFACE_HEADER_VERSION              0x00000000

class ReadDeviceObject : public QObject
{
    Q_OBJECT
public:
    //addres and waribels with curent addres
    struct ReadChanale {
        VarChannel* chanale;
        uint32_t offset;
    };
    struct ReadAddres {
        QVector<struct ReadChanale> vectorChanales;//varivels with offset for this addres
        uint32_t addres;//begin addres read
        uint32_t readSize;//read addresses
    };

    /**
     * @brief isFileDevice - is read from file (if true soft will not save data in file)
     * @return
     */
    virtual bool isFileDevice() = 0;

    /**
     * @brief initDevise - init device (exec when push connect button) read var adresses and prepear read data
     * @param channels - list of varibels
     * @return - 0 if OK else return error code
     */
//    virtual int initDevise(QVector<VarChannel*> *channels) = 0;
    virtual int initDevise(QVector<struct ReadDeviceObject::ReadAddres> readSeuqence) = 0;

    /**
     * @brief stopDev - disconnect from device
     */
    virtual void stopDev() = 0;

    /**
     * @brief execReadDevice - do read from instance
     * @return - 0 if OK else return error code
     */
    virtual int execReadDevice() = 0;

    /**
     * @brief writeDataDevice - write raw data into memory
     * @param data - raw data values
     * @param location - addres located varibles
     * @return - 0 if OK else return error code
     */
    virtual int writeDataDevice(uint32_t data, varloc_location_t location) {return -1;}

    /**
     * @brief getReadDevConfigWidget - get configure widget
     * @return - configure widget for curent device
     */
    virtual QWidget* getReadDevConfigWidget() = 0;

signals:
    void addressesReed(uint32_t addres, QVector<uint8_t> data);
    void addressesReedWithTime(uint32_t addres, QVector<uint8_t> data, QDateTime time);
};

class SaveDeviceObject : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief initDevise - init device (exec when push connect button) read var adresses and prepear save data
     * @param channels - list of varibels
     * @return - 0 if OK else return error code
     */
//    virtual int initDevise(QVector<VarChannel*> *channels) = 0;
    virtual int initDevise(QVector<struct ReadDeviceObject::ReadAddres> readSeuqence) = 0;

    /**
     * @brief stopDev - disconnect from device
     */
    virtual void stopDev() = 0;

    /**
     * @brief execSaveDevice - do save varibels should exec after read waribels and append just last value
     * @return - 0 if OK else return error code
     */
    virtual int execSaveDevice(QVector<QPair<uint32_t,QVector<uint8_t>>> saveSequence) = 0;

    /**
     * @brief getSaveDevConfigWidget - get configure widget
     * @return - configure widget for curent device
     */
    virtual QWidget* getSaveDevConfigWidget() = 0;
};


/************************************************************************/
/* BEGIN init plugins */
/************************************************************************/
/**
 * @brief The ReadDeviceInterfacePlugin class - plugin for read data from device
 */
class ReadDeviceInterfacePlugin
{
public:
    /**
     * @brief getVersion - get version builded library(SHOULDN'T be implemented)
     * @return - ersion builded library
     */
    virtual uint32_t getVersion() {return READ_WRITE_DEVICE_INTERFACE_HEADER_VERSION;}

    /**
     * @brief createDeviceObject - make new device object
     * @return - plot widget (alocate new QWidget)
     */
    virtual ReadDeviceObject* createReadDeviceObject() = 0;

    /**
     * @brief getName - get plugin name
     * @return - plugin name
     */
    virtual QString getName() { return QString(""); }
};

/**
 * @brief The ReadDeviceInterfacePlugin class - plugin for save data
 */
class SaveDeviceInterfacePlugin
{
public:
    /**
     * @brief getVersion - get version builded library(SHOULDN'T be implemented)
     * @return - ersion builded library
     */
    virtual uint32_t getVersion() {return READ_WRITE_DEVICE_INTERFACE_HEADER_VERSION;}

    /**
     * @brief createDeviceObject - make new device object
     * @return - plot widget (alocate new QWidget)
     */
    virtual SaveDeviceObject* createWriteDeviceObject() = 0;

    /**
     * @brief getName - get plugin name
     * @return - plugin name
     */
    virtual QString getName() { return QString(""); }
};

// registrate plugin
QT_BEGIN_NAMESPACE
#define ReadDeviceInterfacePlugin_iid "org.qt-project.S.debuger.ReadDeviceInterfacePlugin"
Q_DECLARE_INTERFACE(ReadDeviceInterfacePlugin, ReadDeviceInterfacePlugin_iid)
#define SaveDeviceInterfacePlugin_iid "org.qt-project.S.debuger.SaveDeviceInterfacePlugin"
Q_DECLARE_INTERFACE(SaveDeviceInterfacePlugin, SaveDeviceInterfacePlugin_iid)
QT_END_NAMESPACE


#endif // READDEVICE_H
