#ifndef SIMPLEREADER_H
#define SIMPLEREADER_H

extern "C" {
#include "stlink.h"
#include <usb.h>
}

#include <QObject>
#include <QMap>
#include "varchannel.h"
class SimpleReader : public QObject
{
    Q_OBJECT
public:
    explicit SimpleReader(QObject *parent = nullptr);
    void loadChannels(QVector<VarChannel *>  *channels);
    int connect();
    bool isConnected();
    int readData();
private:
    QMap<uint32_t, QVector<VarChannel*>*>   read_sequence;
    stlink_t*                               stlink;

signals:

};

#endif // SIMPLEREADER_H
