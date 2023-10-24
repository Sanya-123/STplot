#ifndef STMSTUDIOFILEDEVICE_H
#define STMSTUDIOFILEDEVICE_H

#include "readwritedevice.h"
#include <QFile>
#include <QDateTime>

class STMstudioFileDevice : public SaveDeviceObject, ReadDeviceObject
{
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

private:
    QWidget *configReadWidget;
    QWidget *configSaveWidget;
    QFile device;
    QDateTime startTime;
    QVector<struct ReadDeviceObject::ReadAddres> readSeuqence;
    bool isReadMode;//as it class use for save and load dataI should know witch type is now
    bool isWriteMode;
};

#endif // STMSTUDIOFILEDEVICE_H
