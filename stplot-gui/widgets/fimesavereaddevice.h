#ifndef FIMESAVEREADDEVICE_H
#define FIMESAVEREADDEVICE_H

#include <QObject>
#include "varchannel.h"

class FimeSaveReadDevice : public QObject
{
    Q_OBJECT
public:
    explicit FimeSaveReadDevice(QObject *parent = nullptr);
    ~FimeSaveReadDevice();

public slots:
    int saveChanalesData(QVector<VarChannel*> *channels, QDateTime startTime = QDateTime::currentDateTime());
    int loadChanalesData(QVector<VarChannel*> *channels);

signals:

};

#endif // FIMESAVEREADDEVICE_H
