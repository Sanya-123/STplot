#ifndef READMANAGER_H
#define READMANAGER_H

#include <QObject>
#include "readwritedevice.h"
#include <QThread>
#include "readloop.h"
#include <QMap>
#include "fimesavereaddevice.h"
#include "stmstudiofiledevice.h"

class ReadManager : public QObject
{
    Q_OBJECT
public:
    explicit ReadManager(QObject *parent = nullptr);

    int runReadLoop(QVector<VarChannel*> *channels, QVector<VarChannel*> *mathChannels = nullptr);

    static QVector<ReadDeviceObject::ReadAddres> calcReadSeuqence(QVector<VarChannel*> *channels);

    void setReadDevicece(ReadDeviceObject *newReadDevicece);

    void addSaveDevice(SaveDeviceObject *newSaveDevicece);

public slots:
    void stopRead();
    void requestWriteData(uint32_t data, varloc_location_t location);

signals:
    void stopingRead();
    void beginStopRead();

private slots:
    void addresRead(uint32_t addres, QVector<uint8_t> data);
    void addressesReedWithTime(uint32_t addres, QVector<uint8_t> data, QDateTime time);
    void decodedDataWithTime(QVector<float> data, QDateTime time);
    void mathDataWithTime(QVector<float> data, QDateTime time);
    void stopReadLoop();

private:
    QVector<ReadDeviceObject::ReadAddres> readSeuqencs;
    QMap<uint32_t, ReadDeviceObject::ReadAddres> readSeuqencsMap;
    QVector<VarChannel*> *channels;
    QVector<VarChannel*> *mathChannels;
    ReadLoop *loop;
    QThread readLoopThread;
    QVector<SaveDeviceObject*> saveDeviceces;
    ReadDeviceObject *readDevicece;
//    FimeSaveReadDevice saveLoader;
    QDateTime startTime;
};

#endif // READMANAGER_H
