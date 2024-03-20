#ifndef STMSTUDIOFILEDEVICE_H
#define STMSTUDIOFILEDEVICE_H

#include "readwritedevice.h"
#include <QFile>
#include <QDateTime>
#include <QWidget>

class STMstudioFileDevice : public SaveDeviceObject, ReadDeviceObject
{
//    Q_OBJECT
public:
    STMstudioFileDevice();
    ~STMstudioFileDevice();
    bool isFileDevice();
    int initDevise(QVector<struct ReadDeviceObject::ReadAddres> readSeuqence);
    void stopDev();
    int execSaveDevice(QVector<QPair<uint32_t,QVector<uint8_t>>> saveSequence);
    int execReadDevice();
    QWidget* getReadDevConfigWidget();
    QWidget* getSaveDevConfigWidget();

    int readFileDevice(QVector<VarChannel *> chanales);

private:
    QWidget *configReadWidget;//TODO
    QWidget *configSaveWidget;//TODO
    QFile device;
    QDateTime startTime;
    QVector<struct ReadDeviceObject::ReadAddres> readSeuqence;
    bool isReadMode;//as it class use for save and load dataI should know witch type is now
    bool isWriteMode;
    QVector<uint32_t> masks;
    QVector<varloc_location_t> locations;
};

#endif // STMSTUDIOFILEDEVICE_H
