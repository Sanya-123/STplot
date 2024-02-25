#include "fimesavereaddevice.h"
#include <QFile>

FimeSaveReadDevice::FimeSaveReadDevice(QObject *parent)
    : QObject{parent}
{

}

FimeSaveReadDevice::~FimeSaveReadDevice()
{

}

int FimeSaveReadDevice::saveChanalesData(QVector<VarChannel *> *channels, QDateTime startTime)
{
    QFile saveFile;
    saveFile.setFileName("log_" + startTime.toString("yyyy_MM_dd__hh_mm_ss") +  ".txt");
    if(!saveFile.open(QIODevice::WriteOnly))
        return -1;

    if(channels->size() == 0)
        return 0;

    QString header;
    header.append("Syntax version=4\n");
    header.append(startTime.toString("ddd MMMM dd hh:mm:ss yyyy") + "\n");
    header.append("Starting process (10000000 clocks per sec)\n");//?
    header.append("\n");
    header.append("LogRawData=false\n");//?
    header.append("VarHeader=1\n");

    //generate header data names
    QString names("D:\t");
    QString addresses("D:\ttime(ms)");
    for(int i = 0; i < channels->size(); i++)
    {
        names.append("\t" + channels->at(i)->displayName());

        varloc_location_t loc = channels->at(i)->getLocation();
        addresses.append("\t(0x" + QString::number(loc.address.base + loc.address.offset_bits/8, 16).rightJustified(8, 0) + ",6)");
    }
    header.append(names + "\n");
    header.append(addresses + "\n");

    if(saveFile.write(header.toLocal8Bit()) == -1)
        return -1;

    int sizeBuffer = channels->at(0)->getBufferSize();

    for(int i = 1; i < channels->size(); i++)//looking for a lowest valye
    {
        if(channels->at(i)->getBufferSize() < sizeBuffer)
            sizeBuffer = channels->at(i)->getBufferSize();
    }


    for(int i = 0; i < sizeBuffer; i++)
    {
        QString values("D:\t");
        VarValue val = channels->at(0)->getValue(i);
        values.append(QString::number(val.qtime.msecsSinceStartOfDay()));
        for(int j = 0; j < channels->size(); j++)//looking for a lowest valye
        {
            val = channels->at(j)->getValue(i);
            values.append("\t" + QString::number(val.value));
        }
        values.append("\n");

        if(saveFile.write(values.toLocal8Bit()) == -1)
            return -1;

    }

    return 0;

}

int FimeSaveReadDevice::loadChanalesData(QVector<VarChannel *> *channels, QString fileName)
{

    QFile readFile;
    readFile.setFileName(fileName);
    if(!readFile.open(QIODevice::ReadOnly))
        return -1;

    //read header
    readFile.readLine();
    QByteArray dateTime = readFile.readLine();
    readFile.readLine();
    readFile.readLine();
    readFile.readLine();
    readFile.readLine();
    QByteArray names = readFile.readLine();
    QByteArray addresses = readFile.readLine();


    return -1;
    return 0;
}
