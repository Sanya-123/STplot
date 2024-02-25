#include "stmstudiofiledevice.h"

STMstudioFileDevice::STMstudioFileDevice()
{

}

STMstudioFileDevice::~STMstudioFileDevice()
{

}

bool STMstudioFileDevice::isFileDevice()
{
    return true;
}

int STMstudioFileDevice::initDevise(QVector<struct ReadDeviceObject::ReadAddres> readSeuqence)
{
    isReadMode = false;
    isWriteMode = false;
    this->readSeuqence = readSeuqence;
    device.setFileName("test.txt");
    if(!device.open(QIODevice::ReadWrite))
        return -1;


    startTime = QDateTime::currentDateTime();

    return 0;
}

void STMstudioFileDevice::stopDev()
{
    isReadMode = false;
    isWriteMode = false;
    device.close();
}

int STMstudioFileDevice::execSaveDevice(QVector<QPair<uint32_t,QVector<uint8_t>>> saveSequence)
{//BUG rebork it as save row data
    if(isReadMode)
        return -1;

    if(!isWriteMode)//if it is first exec
    {
        //save header
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
        for(int i = 0; i < readSeuqence.size(); i++)
        {
            for(int j = 0; j < readSeuqence[i].vectorChanales.size(); j++)
            {
                //TODO check how it works in anouther thread
                names.append("\t" + readSeuqence[i].vectorChanales[j].chanale->displayName());

                varloc_location_t loc = readSeuqence[i].vectorChanales[j].chanale->getLocation();
                addresses.append("\t(0x" + QString::number(loc.address.base + loc.address.offset_bits/8, 16).rightJustified(8, 0) + ",6)");
            }
        }
        header.append(names + "\n");
        header.append(addresses + "\n");

        if(device.write(header.toLocal8Bit()) == -1)
            return -1;

        isWriteMode = true;
    }

    QString values("D:\t");
    values.append(QString::number(QDateTime::currentMSecsSinceEpoch() - startTime.toMSecsSinceEpoch()));

    union __attribute__((packed)){
        uint8_t _8[4];
        uint32_t _32;
    }combiner;

    for(int i = 0; i < readSeuqence.size(); i++)
    {
        if(i >= saveSequence.size())
            break;


        ReadDeviceObject::ReadAddres addresSequence = readSeuqence[i];

        for(int j = 0 ; j < saveSequence[i].second.size()/4; j++)
        {
            combiner._32 = 0;
            memcpy(combiner._8, saveSequence[i].second.data() + addresSequence.vectorChanales[j].offset, /*addresSequence.vectorChanales[j].varSize*/4);
            //NOTE add types if you vould like make it compareble with STMstudio

            values.append("\t" + QString::number(combiner._32));
        }
    }

    if(device.write(values.toLocal8Bit()) == -1)
        return -1;

    return 0;
}

int STMstudioFileDevice::execReadDevice()
{//BUG rebork it as read row data
    if(isWriteMode)
        return -1;

    if(!isReadMode)//if it is first exec
    {
        //read header
        device.readLine();
        QByteArray dateTime = device.readLine();
        device.readLine();
        device.readLine();
        device.readLine();
        device.readLine();
        QByteArray names = device.readLine();
        QByteArray addresses = device.readLine();

        //check names
        QString namesShouldBe("D:\t");
        for(int i = 0; i < readSeuqence.size(); i++)
        {
            for(int j = 0; j < readSeuqence[i].vectorChanales.size(); j++)
            {
                //TODO check how it works in anouther thread
                namesShouldBe.append("\t" + readSeuqence[i].vectorChanales[j].chanale->displayName());
            }
        }
        namesShouldBe.append("\n");
        if(namesShouldBe.toLatin1() != names)//check names
            return -1;

        isReadMode = true;

        //get some how startTime
        startTime;
    }

    QByteArray values = device.readLine();
    if(values.size() <= 0)
        return -1;

    QList<QByteArray> listVaribels = values.split('\t');
    if(listVaribels.size() <= 2)
        return -1;

    int numberVaribles = 2;

    qint64 timeSample_ms = listVaribels[1].toLongLong();

    union __attribute__((packed)){
        uint8_t _8[4];
        uint32_t _32;
    }combiner;

    for(int i = 0; i < readSeuqence.size(); i++)
    {//BUG this cycle will note work, try to make it with read addresses from header
        if(numberVaribles >= listVaribels.size())
            break;

        ReadDeviceObject::ReadAddres addresSequence = readSeuqence[i];

        QVector<uint8_t> reedData(addresSequence.readSize);

        for(int j = 0 ; j < addresSequence.readSize/4; j++)
        {
            if(numberVaribles >= listVaribels.size())
                break;

            combiner._32 = listVaribels[numberVaribles++].toUInt();
            memcpy(reedData.data() + 4*j, combiner._8, 4);
        }

        emit addressesReedWithTime(addresSequence.addres, reedData, startTime.addMSecs(timeSample_ms));
    }


    return 0;
}

QWidget *STMstudioFileDevice::getReadDevConfigWidget()
{
    return configReadWidget;
}

QWidget *STMstudioFileDevice::getSaveDevConfigWidget()
{
    return configSaveWidget;
}
