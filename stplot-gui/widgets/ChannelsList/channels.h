#ifndef CHANNELS_H
#define CHANNELS_H

#include <QWidget>
#include "varloc.h"
#include "channelmodel.h"

namespace Ui {
class Channels;
}

class Channels : public QWidget
{
    Q_OBJECT

public:
    explicit Channels(QWidget *parent = nullptr);
    ~Channels();

public slots:
    void add_channel(varloc_node_t* node);

private:
    Ui::Channels *ui;
    ChannelModel *m_channelModel;
    QVector<VarChannel*> *m_channels;
};

#endif // CHANNELS_H
