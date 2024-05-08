#ifndef STMSTUDIOFILEDEVICE_H
#define STMSTUDIOFILEDEVICE_H

#include "readwritedevice.h"
#include <QFile>
#include <QDateTime>
#include <QWidget>
#include <QLineEdit>

class STMstudioFileDevice : public SaveDeviceObject
{
    Q_OBJECT
public:
    STMstudioFileDevice();
    ~STMstudioFileDevice();
    bool isFileDevice();
    int initDevise(QVector<struct ReadDeviceObject::ReadAddres> readSeuqence);
    void stopDev();
    int execSaveDevice(QVector<QPair<uint32_t,QVector<uint8_t>>> saveSequence, QDateTime time);
    int execSaveDevice(QList<QString> chanaleNames, QVector<float> listDecoded, QDateTime time);
    int execReadDevice();
    QWidget* getReadDevConfigWidget();
    QWidget* getSaveDevConfigWidget();

    int readFileDevice(QVector<VarChannel *> chanales, QVector<QTime> *readTimes = nullptr);

private slots:
    void openSelectFile();
    int initSaveFile();

private:
    QWidget *configReadWidget;
    QLineEdit* fileRead;
    QWidget *configSaveWidget;
    QFile device;
    QDateTime startTime;
    QVector<struct ReadDeviceObject::ReadAddres> readSeuqence;
    bool isReadMode;//as it class use for save and load dataI should know witch type is now
    bool isWriteMode;
    QVector<varloc_location_t> locations;
};

#endif // STMSTUDIOFILEDEVICE_H
