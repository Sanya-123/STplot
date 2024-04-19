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
//    void setChannels(QVector<VarChannel *> *newChannels);

    void setReadSequence(const QVector<ReadDeviceObject::ReadAddres> &newReadSequence,
                         /*use for calc decod list*/QVector<VarChannel *> *channels = nullptr);

    void updateMathChanales(QVector<VarChannel *> *mathChanales);

    static QMap<QString, float> fillMapValues(QStringList chanaleNames, QVector<float> values);
    static QVector<float> calcMathChanales(QMap<QString, float> mapChanales, QVector<QPair<QString,QString>> *listMathChanales);
    static QVector<float> calcMathChanales(QList<QString> listChanalesName, QVector<float> listChanalesValues, QVector<QPair<QString,QString>> *listMathChanales);


public slots:
    void readLoop();
    void stopLoop();
    void requestWriteData(uint32_t data, varloc_location_t location);

private:
    void calcDecodList(QVector<VarChannel *> *channels);
    QVector<float> decodSavedSequence();

private slots:
    void saveReedSequence(uint32_t addres, QVector<uint8_t> data, QDateTime time);

signals:
    void stopedLoop();
    void addressesReed(uint32_t addres, QVector<uint8_t> data);
    void addressesReedWithTime(uint32_t addres, QVector<uint8_t> data, QDateTime time);
    void decodedDataWithTime(QVector<float> data, QDateTime time);
    void mathDataWithTime(QVector<float> data, QDateTime time);

private:
    QVector<SaveDeviceObject*> *saveDeviceces;
    ReadDeviceObject* readDevicec;
//    QVector<VarChannel*> *channels;
    bool stopSignal;
    QVector<ReadDeviceObject::ReadAddres> readSequence;
    QVector<QPair<uint32_t,QVector<uint8_t>>> saveSequence;
    QVector<QPair<uint32_t,varloc_location_t>> requestedWriteData;

    //map decod chanales bitmap and locations
    QVector<varloc_location_t> decodeList;

    //use for calc math
    QList<QString> chanaleDisplayNames;
    QList<QString> chanaleNames;
    QVector<QPair<QString,QString>> listMathChanales;


};

#endif // READLOOP_H
