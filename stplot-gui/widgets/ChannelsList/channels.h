#ifndef CHANNELS_H
#define CHANNELS_H

#include <QWidget>
#include "varloc.h"
#include "channelmodel.h"
#include <QSettings>
#include "chanaleitemdelegate.h"

namespace Ui {
class Channels;
}

class Channels : public QWidget
{
    Q_OBJECT

public:
    explicit Channels(QWidget *parent = nullptr);
    ~Channels();

    void saveSettings(QSettings *settings);
    void restoreSettings(QSettings *settings);

    QVector<VarChannel*> *getListChanales() const;

public slots:
    void add_channel(varloc_node_t* node);
    void addPlot();
    void deletePlot(int number);
    void setPlotName(int number, QString name);

signals:
    void addingChanaleToPlot(VarChannel* var, int numPlot, bool en);

private slots:
    void on_pushButton_clicked();

private:
    Ui::Channels *ui;
    ChannelModel *m_channelModel;
    QVector<VarChannel*> *m_channels;
    ChanaleItemDelegate iteamDeclarater;
    //sequnce sent color for new gruph
    QList<QColor> colorSetSequese;
    int curentColorSet;
    int curentDotStyle;
};

#endif // CHANNELS_H
