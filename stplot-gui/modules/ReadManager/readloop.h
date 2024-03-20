#ifndef READLOOP_H
#define READLOOP_H

#include <QObject>
#include "readwritedevice.h"

class ReadLoop : public QObject
{
    Q_OBJECT
public:
    explicit ReadLoop(QObject *parent = nullptr);

    void setSaveDeviceces(QVector<SaveDeviceObject *> *newSaveDeviceces);
    void setReadDevicec(ReadDeviceObject *newReadDevicec);
    void setChannels(QVector<VarChannel *> *newChannels);

    void setReadSequence(const QVector<ReadDeviceObject::ReadAddres> &newReadSequence);

public slots:
    void readLoop();
    void stopLoop();
    void requestWriteData(uint32_t data, varloc_location_t location);

private slots:
    void saveReedSequence(uint32_t addres, QVector<uint8_t> data, QDateTime time);

signals:
    void stopedLoop();
    void addressesReed(uint32_t addres, QVector<uint8_t> data);
    void addressesReedWithTime(uint32_t addres, QVector<uint8_t> data, QDateTime time);

private:
    QVector<SaveDeviceObject*> *saveDeviceces;
    ReadDeviceObject* readDevicec;
    QVector<VarChannel*> *channels;
    bool stopSignal;
    QVector<ReadDeviceObject::ReadAddres> readSequence;
    QVector<QPair<uint32_t,QVector<uint8_t>>> saveSequence;
    QVector<QPair<uint32_t,varloc_location_t>> requestedWriteData;


};

#endif // READLOOP_H
