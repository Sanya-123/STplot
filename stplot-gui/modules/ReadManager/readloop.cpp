#include "readloop.h"
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>
#include <QScriptEngine>

ReadLoop::ReadLoop(QObject *parent)
    : QObject{parent}, readDevicec(nullptr), saveDeviceces(nullptr)/*, channels(nullptr)*/
{
}

void ReadLoop::readLoop()
{
    stopSignal = false;
    if(readDevicec == nullptr/* || channels == nullptr*/)
    {
        emit stopedLoop();
        return;
    }

    saveSequence.clear();
    requestedWriteData.clear();

    bool isFileDev = readDevicec->isFileDevice();

//    readDevicec->moveToThread(this->thread());
//    for(int i = 0; i < saveDeviceces->size(); i++)
//        saveDeviceces->at(i)->moveToThread(this->thread());

    connect(readDevicec, SIGNAL(addressesReedWithTime(uint32_t,QVector<uint8_t>,QDateTime)), this, SIGNAL(addressesReedWithTime(uint32_t,QVector<uint8_t>,QDateTime)));
    connect(readDevicec, SIGNAL(addressesReedWithTime(uint32_t,QVector<uint8_t>,QDateTime)), this, SLOT(saveReedSequence(uint32_t,QVector<uint8_t>,QDateTime)));

    try {

        //try init dev
//        int res = readDevicec->initDevise(channels);
        int res = readDevicec->initDevise(readSequence);
        if(res != 0)//error read device
            throw res;

        if(!isFileDev && saveDeviceces != nullptr)
        {
            for(int i = 0; i < saveDeviceces->size(); i++)
                saveDeviceces->at(i)->initDevise(readSequence);
        }

//        QElapsedTimer timer;
//        timer.start();

        QVector<QMap<QString, float>> vectorOldValues;

        //main loop of read data
        do{
            //read value
            int resRead = readDevicec->execReadDevice();
            if(resRead != 0)//error read device
                throw resRead;


            QDateTime _t = QDateTime::currentDateTime();

            if(decodeList.size() != 0)
            {
                QVector<float> listDecoded = decodSavedSequence();
                emit decodedDataWithTime(listDecoded, _t);

                //calc math chanales
                QVector<float> listMathValues = calcMathChanales(chanaleDisplayNamesReplased, listDecoded,
                                                                 &listMathChanales, &vectorOldValues);
                emit mathDataWithTime(listMathValues, _t);

                //savde data in file
                if(!isFileDev && saveDeviceces != nullptr)
                {
                    for(int i = 0; i < saveDeviceces->size(); i++)
                        saveDeviceces->at(i)->execSaveDevice(chanaleNames, listDecoded, _t);
                }
            }
            else
            {
                //savde data in file
                if(!isFileDev && saveDeviceces != nullptr)
                {
                    for(int i = 0; i < saveDeviceces->size(); i++)
                        saveDeviceces->at(i)->execSaveDevice(saveSequence, _t);
                }
            }

            //decode saved sequence

            saveSequence.clear();

            //write data
            while(requestedWriteData.size())
            {
                readDevicec->writeDataDevice(requestedWriteData[0].first, requestedWriteData[0].second);
                requestedWriteData.removeFirst();
            }
            QThread::msleep(2);

        }while(stopSignal == false);

//        qDebug () << "spendTime:" << timer.elapsed();

    } catch (int err) {
        Q_UNUSED(err);
        qDebug() << err;
    }
    //stop
    //stop read
    readDevicec->stopDev();
    //stao all dev if it nesasary
    if(!isFileDev && saveDeviceces != nullptr)
    {
        for(int i = 0; i < saveDeviceces->size(); i++)
            saveDeviceces->at(i)->stopDev();
    }

    disconnect(readDevicec, SIGNAL(addressesReedWithTime(uint32_t,QVector<uint8_t>,QDateTime)), this, SIGNAL(addressesReedWithTime(uint32_t,QVector<uint8_t>,QDateTime)));
    disconnect(readDevicec, SIGNAL(addressesReedWithTime(uint32_t,QVector<uint8_t>,QDateTime)), this, SLOT(saveReedSequence(uint32_t,QVector<uint8_t>,QDateTime)));
    saveSequence.clear();
    emit stopedLoop();
}

void ReadLoop::setSaveDeviceces(QVector<SaveDeviceObject *> *newSaveDeviceces)
{
    saveDeviceces = newSaveDeviceces;
}
void ReadLoop::setReadDevicec(ReadDeviceObject *newReadDevicec)
{
    readDevicec = newReadDevicec;
}

void ReadLoop::stopLoop()
{
    stopSignal = true;
}

void ReadLoop::requestWriteData(uint64_t data, varloc_location_t location)
{
    requestedWriteData.append(qMakePair(data, location));
}

void ReadLoop::saveReedSequence(uint32_t addres, QVector<uint8_t> data, QDateTime time)
{
    Q_UNUSED(time);
    saveSequence.append(qMakePair(addres, data));
}

void ReadLoop::setReadSequence(const QVector<ReadDeviceObject::ReadAddres> &newReadSequence, QVector<VarChannel *> *channels)
{
    readSequence = newReadSequence;
    calcDecodList(channels);
}

void ReadLoop::updateMathChanales(QVector<VarChannel *> *mathChanales)
{
    listMathChanales.clear();

    if(mathChanales != nullptr)
    {
        for(int i = 0; i < mathChanales->size(); i++)
        {
            QString name = mathChanales->at(i)->displayName();
            QString script = mathChanales->at(i)->script();
//            script = script.replace(".", "_");
            listMathChanales.append(qMakePair(name, script));
        }
    }
}

QMap<QString, float> ReadLoop::fillMapValues(QStringList chanaleNames, QVector<float> values)
{
    QMap<QString, float> res;

    for(int i = 0; i < chanaleNames.size(); i++)
    {
        if(i >= values.size())
            break;

        res[chanaleNames[i]] = values[i];
    }

    return res;
}

QVector<float> ReadLoop::calcMathChanales(QMap<QString, float> mapChanales, QVector<QPair<QString, QString>> *listMathChanales,
                                          QVector<QMap<QString, float> > *vectorOldValues)
{
//    QList<QString> chanaleNames
//    QScriptEngine myEngine;
//    for(mapChanales.)
//    myEngine.globalObject().setProperty("foo", 123);

    QList<QString> listChanalesName = mapChanales.keys();
    QVector<float> listChanalesValues;

    for(int i = 0; i < listChanalesName.size(); i++)
    {
        listChanalesValues.append(mapChanales[listChanalesName[i]]);
    }

    for(int i = 0; i < listChanalesName.size(); i++)
    {
        REPLEASE_DOT_VAR_NAME(listChanalesName[i]);
    }

    return calcMathChanales(listChanalesName, listChanalesValues, listMathChanales, vectorOldValues);
}

QVector<float> ReadLoop::calcMathChanales(QList<QString> listChanalesNameReplaced, QVector<float> listChanalesValues,
                                          QVector<QPair<QString,QString>> *listMathChanales, QVector<QMap<QString, float>> *vectorOldValues)
{
    //https://doc.qt.io/qt-5/qtscript-index.html

    QVector<float> res;

    if(listMathChanales == nullptr)
        return res;

    if(listMathChanales->size() == 0)
        return res;

    QMap<QString, float> mapOldData;

    QScriptEngine myEngine;
    for(int i = 0; i < listChanalesNameReplaced.size(); i++)
    {
        if(i >= listChanalesValues.size())
            break;

        myEngine.globalObject().setProperty(listChanalesNameReplaced[i], listChanalesValues[i]);
        mapOldData[listChanalesNameReplaced[i]] = listChanalesValues[i];
    }

    if(vectorOldValues != nullptr)
    {
        //NOTE it is will break if try to work on different threads
        static QVector<QMap<QString, float>> *_vectorOldValues = nullptr;
        static QMap<QString, float> *_mapOldData = nullptr;

        _vectorOldValues = vectorOldValues;
        _mapOldData = &mapOldData;

        //prevoius value function
        auto prevValue = [/*vectorOldValues, mapOldData*/] (QScriptContext *context, QScriptEngine *engine) -> QScriptValue
        {
            Q_UNUSED(engine);
            QScriptValue chanaleName = context->argument(0);
            QScriptValue delayValue = context->argument(1);
            QScriptValue res(0.0);
            if(delayValue.toUInt32() > 0)
            {
                if(_vectorOldValues->size() >= delayValue.toUInt32())
                {
                    //find chanale
                    if(_vectorOldValues->at(delayValue.toUInt32() - 1).contains(chanaleName.toString()))
                        res = _vectorOldValues->at(delayValue.toUInt32() - 1)[chanaleName.toString()];
                }
            }
            else
            {
                if(_mapOldData->contains(chanaleName.toString()))
                    res = _mapOldData->value(chanaleName.toString());
            }


            return res;
        };
        QScriptEngine::FunctionSignature prevValueLambdaPtr = prevValue;
        QScriptValue fun = myEngine.newFunction(prevValueLambdaPtr);
        myEngine.globalObject().setProperty("prevValue", fun);
    }

    {//unsigned -> signed

        auto uintToInt = [/*vectorOldValues, mapOldData*/] (QScriptContext *context, QScriptEngine *engine) -> QScriptValue
        {
            Q_UNUSED(engine);
            QScriptValue value = context->argument(0);
            uint16_t bitsSize = context->argument(1).toUInt16();
            if(bitsSize == 0 || bitsSize > 32)
                return  QScriptValue(0.0);

            uint64_t maxSigned = 1 << (bitsSize - 1);
            uint64_t maxUnsigned = 1 << (bitsSize);
            uint32_t valU = value.toUInt32();
                if(valU >= maxSigned)
                    return QScriptValue((valU - maxUnsigned)*1.0);
            return value;
        };
        QScriptEngine::FunctionSignature uintToIntLambdaPtr = uintToInt;
        QScriptValue fun = myEngine.newFunction(uintToIntLambdaPtr);
        myEngine.globalObject().setProperty("uintToInt", fun);
    }


    for(int i = 0; i < listMathChanales->size(); i++)
    {
        QString script = listMathChanales->at(i).second;
        float valueScript = myEngine.evaluate(script).toNumber();
        res.append(valueScript);
        mapOldData[listMathChanales->at(i).first] = valueScript;
    }

    //save old values
    if(vectorOldValues != nullptr)
    {
        vectorOldValues->prepend(mapOldData);
        if(DEPH_OLD_DATA >= 0)
            while(vectorOldValues->size() > DEPH_OLD_DATA)
                vectorOldValues->removeLast();
    }

    return res;
}

void ReadLoop::calcDecodList(QVector<VarChannel *> *channels)
{
    decodeList.clear();
    chanaleDisplayNames.clear();
    chanaleNames.clear();
    chanaleDisplayNamesReplased.clear();

    if(channels != nullptr)
    {
        for(int i = 0; i < channels->size(); i++)
        {
            QString chanaleName = channels->at(i)->displayName();
            chanaleDisplayNames << chanaleName;
            chanaleNames << channels->at(i)->getName();
            varloc_location_t loc = channels->at(i)->getLocation();
            decodeList.append(loc);
            REPLEASE_DOT_VAR_NAME(chanaleName);
            chanaleDisplayNamesReplased << chanaleName;
        }
    }
}

QVector<float> ReadLoop::decodSavedSequence()
{
    int numbegChanale = 0;
    QVector<float> res;
    union __attribute__((packed)){
        uint8_t _8[8];
        uint64_t _64;
    }combiner;

    for(int i = 0; i < readSequence.size(); i++)
    {
        if(i >= saveSequence.size())
            break;

        if(numbegChanale >= decodeList.size())
            break;

        ReadDeviceObject::ReadAddres addresSequence = readSequence[i];

        for(int j = 0 ; j < addresSequence.vectorChanales.size(); j++)
        {
            if(numbegChanale >= decodeList.size())
                break;

            combiner._64 = 0;
            memcpy(combiner._8, saveSequence[i].second.data() + addresSequence.vectorChanales[j].offset, /*addresSequence.vectorChanales[j].varSize*/8);

            float valuesFloat = VarChannel::decode_value(combiner._64, decodeList[numbegChanale]);

            res.append(valuesFloat);

            numbegChanale++;
        }
    }

    return res;
}
//void ReadLoop::setChannels(QVector<VarChannel *> *newChannels)
//{
//    channels = newChannels;
//}
