#include "stmstudiofiledevice.h"
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QPushButton>
#include <QFileDialog>

STMstudioFileDevice::STMstudioFileDevice()
{
    stopDev();
    configReadWidget = new QWidget;
    QLabel *labelFile = new QLabel("File:", configReadWidget);
    fileRead = new QLineEdit(configReadWidget);
    QPushButton *buttomSelectFile = new QPushButton("open", configReadWidget);
    connect(buttomSelectFile, SIGNAL(clicked(bool)), this, SLOT(openSelectFile()));


    QHBoxLayout* layout = new QHBoxLayout(configReadWidget);
    layout->addWidget(labelFile);
    layout->addWidget(fileRead);
    layout->addWidget(buttomSelectFile);
}

STMstudioFileDevice::~STMstudioFileDevice()
{
    delete configReadWidget;
}

bool STMstudioFileDevice::isFileDevice()
{
    return true;
}

int STMstudioFileDevice::initDevise(QVector<struct ReadDeviceObject::ReadAddres> readSeuqence)
{
    masks.clear();
    locations.clear();
    isReadMode = false;
    isWriteMode = false;
    startTime = QDateTime::currentDateTime();
    this->readSeuqence = readSeuqence;
//    device.setFileName("test.txt");
//    device.setFileName("log_" + startTime.toString("yyyy_MM_dd__hh_mm_ss") +  ".txt");
//    if(!device.open(QIODevice::ReadWrite))
//        return -1;


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
        device.setFileName("log_" + startTime.toString("yyyy_MM_dd__hh_mm_ss") +  ".txt");
        if(!device.open(QIODevice::ReadWrite))
            return -1;

        masks.clear();
        locations.clear();

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
                names.append("\t" + readSeuqence[i].vectorChanales[j].chanale->getName());

                varloc_location_t loc = readSeuqence[i].vectorChanales[j].chanale->getLocation();
                addresses.append("\t(0x" + QString::number(loc.address.base + loc.address.offset_bits/8, 16).rightJustified(8, 0) + ",6)");
                masks.append(readSeuqence[i].vectorChanales[j].chanale->getMask());
                locations.append(loc);
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

    int numbegChanale = 0;

    for(int i = 0; i < readSeuqence.size(); i++)
    {
        if(i >= saveSequence.size())
            break;


        ReadDeviceObject::ReadAddres addresSequence = readSeuqence[i];

        for(int j = 0 ; j < saveSequence[i].second.size()/4; j++)
        {
            combiner._32 = 0;
            memcpy(combiner._8, saveSequence[i].second.data() + addresSequence.vectorChanales[j].offset, /*addresSequence.vectorChanales[j].varSize*/4);

            //NOTE maybe move decode_value put of out of this module
            float valuesFloat = VarChannel::decode_value(combiner._32, masks[numbegChanale], locations[numbegChanale]);
            //NOTE add types if you vould like make it compareble with STMstudio

            values.append("\t" + QString::number(valuesFloat));


            numbegChanale++;
        }
    }

    values.append("\n");

    if(device.write(values.toLocal8Bit()) == -1)
        return -1;

    return 0;
}

int STMstudioFileDevice::execReadDevice()
{//BUG rebork it as read row data
    return -1;
//    if(isWriteMode)
//        return -1;

//    if(!isReadMode)//if it is first exec
//    {
//        //read header
//        device.readLine();
//        QByteArray dateTime = device.readLine();
//        device.readLine();
//        device.readLine();
//        device.readLine();
//        device.readLine();
//        QByteArray names = device.readLine();
//        QByteArray addresses = device.readLine();

//        //check names
//        QString namesShouldBe("D:\t");
//        for(int i = 0; i < readSeuqence.size(); i++)
//        {
//            for(int j = 0; j < readSeuqence[i].vectorChanales.size(); j++)
//            {
//                //TODO check how it works in anouther thread
//                namesShouldBe.append("\t" + readSeuqence[i].vectorChanales[j].chanale->displayName());
//            }
//        }
//        namesShouldBe.append("\n");
//        if(namesShouldBe.toLatin1() != names)//check names
//            return -1;

//        isReadMode = true;

//        //get some how startTime
//        startTime;
//    }

//    QByteArray values = device.readLine();
//    if(values.size() <= 0)
//        return -1;

//    QList<QByteArray> listVaribels = values.split('\t');
//    if(listVaribels.size() <= 2)
//        return -1;

//    int numberVaribles = 2;

//    qint64 timeSample_ms = listVaribels[1].toLongLong();

//    union __attribute__((packed)){
//        uint8_t _8[4];
//        uint32_t _32;
//    }combiner;

//    for(int i = 0; i < readSeuqence.size(); i++)
//    {//BUG this cycle will note work, try to make it with read addresses from header
//        if(numberVaribles >= listVaribels.size())
//            break;

//        ReadDeviceObject::ReadAddres addresSequence = readSeuqence[i];

//        QVector<uint8_t> reedData(addresSequence.readSize);

//        for(int j = 0 ; j < addresSequence.readSize/4; j++)
//        {
//            if(numberVaribles >= listVaribels.size())
//                break;

//            combiner._32 = listVaribels[numberVaribles++].toUInt();
//            memcpy(reedData.data() + 4*j, combiner._8, 4);
//        }

//        emit addressesReedWithTime(addresSequence.addres, reedData, startTime.addMSecs(timeSample_ms));
//    }


//    return 0;
}

QWidget *STMstudioFileDevice::getReadDevConfigWidget()
{
    return configReadWidget;
}

QWidget *STMstudioFileDevice::getSaveDevConfigWidget()
{
    return configSaveWidget;
}

#include <QDebug>

int STMstudioFileDevice::readFileDevice(QVector<VarChannel *> chanales, QVector<QTime> *readTimes)
{
    if(isWriteMode)
        return -1;

    device.setFileName(fileRead->text());
    if(!device.open(QIODevice::ReadOnly))
        return -2;

    QVector<VarChannel *> readChanales;


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
        names.remove(names.size() - 1, 1);//remove \n
//        names.r

        QList<QByteArray> listName = names.split('\t');

        readChanales.resize(listName.size() - 2);

        //check names and map chanales
        for(int i = 2; i < listName.size(); i++)
        {
            readChanales[i - 2] = nullptr;
            for(int j = 0; j < chanales.size(); j++)
            {
                if(listName[i] == chanales[j]->getName())
                {
                    readChanales[i - 2] = chanales[j];
                }
            }
//
        }

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
//        if(namesShouldBe.toLatin1() != names)//check names
//            return -2;

        isReadMode = true;

        //get some how startTime
        startTime;

        //TODO map addresses
    }

    while(1)
    {
        QByteArray values = device.readLine();
        if(values.size() <= 0)
            return 0;

        QList<QByteArray> listVaribels = values.split('\t');
        if(listVaribels.size() <= 2)
            continue;

        int timeSample_ms = listVaribels[1].toLongLong();

        QTime time = QTime(0, 0);
        time = time.addMSecs(timeSample_ms);
        if(readTimes != nullptr)
            readTimes->append(time);


        for(int i = 2; i < listVaribels.size(); i++)
        {
            readChanales[i - 2]->pushValue(listVaribels[i].toFloat(), time);
        }
    }


    return 0;
}

void STMstudioFileDevice::openSelectFile()
{
    QString file = QFileDialog::getOpenFileName(configReadWidget, tr("Select file"), fileRead->text(), "*");
    if(!file.isEmpty())
        fileRead->setText(file);
}
