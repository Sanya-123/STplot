#ifndef READMANAGER_H
#define READMANAGER_H

#include <QObject>
#include "readwritedevice.h"
#include <QThread>
#include "readloop.h"
#include <QMap>
#include "readwritedevice.h"

class ReadManager : public QObject
{
    Q_OBJECT
public:
    explicit ReadManager(QObject *parent = nullptr);

    int runReadLoop(QVector<VarChannel*> *channels);

    static QVector<ReadDeviceObject::ReadAddres> calcReadSeuqence(QVector<VarChannel*> *channels);

    void setReadDevicece(ReadDeviceObject *newReadDevicece);

public slots:
    void stopRead();

signals:
    void stopingRead();

private slots:
    void addresRead(uint32_t addres, QVector<uint8_t> data);
    void stopReadLoop();

private:
    QVector<ReadDeviceObject::ReadAddres> readSeuqencs;
    QMap<uint32_t, ReadDeviceObject::ReadAddres> readSeuqencsMap;
    QVector<VarChannel*> *channels;
    ReadLoop *loop;
    QThread readLoopThread;
    QVector<SaveDeviceObject*> saveDeviceces;
    ReadDeviceObject *readDevicece;
};

#endif // READMANAGER_H
