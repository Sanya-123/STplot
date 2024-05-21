#ifndef TELEMETRYFILEDEVICE_H
#define TELEMETRYFILEDEVICE_H

#include "readwritedevice.h"
#include <QFile>
#include <QDateTime>
#include <QWidget>
#include <QLineEdit>

class TelemetryFileDevice : public ReadDeviceObject
{
    Q_OBJECT
public:
    TelemetryFileDevice();
    ~TelemetryFileDevice();
    bool isFileDevice();
    int initDevise(QVector<struct ReadDeviceObject::ReadAddres> readSeuqence);
    void stopDev();
    int execReadDevice();
    QWidget* getReadDevConfigWidget();

    int readFileDevice(QVector<VarChannel *> chanales, QVector<QTime> *readTimes = nullptr);

private slots:
    void openSelectFolder();

private:
    int readTelemetryFile(QString filepath, QVector<VarChannel *> chanales, QVector<QTime> *readTimes);
    int readTelemetryDir(QString path, QVector<VarChannel *> chanales, QVector<QTime> *readTimes);

    QWidget *configReadWidget;
    QLineEdit* folderName;
    QWidget *configSaveWidget;
    // QFile device;
    // QDateTime startTime;
    QVector<struct ReadDeviceObject::ReadAddres> readSeuqence;
    // bool isReadMode;//as it class use for save and load dataI should know witch type is now
    // bool isWriteMode;
    QVector<varloc_location_t> locations;
};

#endif // TELEMETRYFILEDEVICE_H
