#ifndef READLOOP_H
#define READLOOP_H

#include <QObject>
#include "readwritedevice.h"

class ReadLoop : public QObject
{
    Q_OBJECT
public:
    explicit ReadLoop(QObject *parent = nullptr);
    void readLoop();

    void setSaveDeviceces(QVector<SaveDeviceObject *> *newSaveDeviceces);
    void setReadDevicec(ReadDeviceObject *newReadDevicec);
    void setChannels(QVector<VarChannel *> *newChannels);

    void setReadSequence(const QVector<ReadDeviceObject::ReadAddres> &newReadSequence);

public slots:
    void stopLoop();

signals:
    void stopedLoop();

private:
    QVector<SaveDeviceObject*> *saveDeviceces;
    ReadDeviceObject* readDevicec;
    QVector<VarChannel*> *channels;
    bool stopSignal;
    QVector<ReadDeviceObject::ReadAddres> readSequence;

};

#endif // READLOOP_H
