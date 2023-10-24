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

private slots:
    void saveReedSequence(uint32_t addres, QVector<uint8_t> data);

signals:
    void stopedLoop();
    void addressesReed(uint32_t addres, QVector<uint8_t> data);

private:
    QVector<SaveDeviceObject*> *saveDeviceces;
    ReadDeviceObject* readDevicec;
    QVector<VarChannel*> *channels;
    bool stopSignal;
    QVector<ReadDeviceObject::ReadAddres> readSequence;
    QVector<QPair<uint32_t,QVector<uint8_t>>> saveSequence;

};

#endif // READLOOP_H
