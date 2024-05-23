#ifndef CHANNELS_H
#define CHANNELS_H

#include <QWidget>
#include "varcommon.h"
#include "channelmodel.h"
#include <QSettings>
#include "chanaleitemdelegate.h"
#include <QSortFilterProxyModel>

namespace Ui {
class Channels;
}

QT_BEGIN_NAMESPACE
class QTreeView;
QT_END_NAMESPACE

class Channels : public QWidget
{
    Q_OBJECT
    enum ListChanaleView{
        ChanalesView = 0,
        MathChanalesView,
        TotalSizeChanalesView
    };
public:
    explicit Channels(QWidget *parent = nullptr);
    ~Channels();

    void saveSettings(QSettings *settings);
    void restoreSettings(QSettings *settings);
    QVector<VarChannel*> *getListChanales() const;
    QVector<VarChannel*> *getListMathChanales() const;

public slots:
    void reloadChannels(varloc_node_t* root);
    void add_channel(varloc_node_t* node);
    void addPlot();
    void deletePlot(int number);
    void setPlotName(int number, QString name);

signals:
    void addingChanaleToPlot(VarChannel* var, int numPlot, bool en);
    void requestWriteData(uint64_t data, varloc_location_t location);
    void requestReplot();

private:
    void saveSettingsChanaleCommon(QSettings *settings, VarChannel* chanale);
    void saveSettingsChanaleLocation(QSettings *settings, VarChannel* chanale);
    void saveSettingsChanaleScript(QSettings *settings, VarChannel* chanale);
    void saveSettingsChanalePlot(QSettings *settings, VarChannel* chanale);

    varloc_location_t restoreSettingsChanaleLocation(QSettings *settings);
    void restoreSettingsChanaleCommon(QSettings *settings,
                                      QString *chanaleName, QString *displayName, int *dotStyle, int *lineStyle,
                                      QColor *lineColor, int *lineWidth,
                                      double *offset, double *mult);
    QString restoreSettingsChanaleScript(QSettings *settings);
    QVector<bool> restoreSettingsChanaleListPlot(QSettings *settings);


private slots:
    void deleteSelectedChanales(ListChanaleView numList);
//    void on_pushButton_clicked();

    void on_pushButton_deleteChanale_clicked();

    void on_pushButton_addMathChanale_clicked();

    void on_pushButton_deleteMathChanale_clicked();
    void openChanaleMenu(const QPoint &point);

private:
    struct
    {
        QTreeView *treeView;
        QVector<VarChannel*> *channels;
        ChannelModel *channelModel;
        QSortFilterProxyModel *chanaleProxyModel;
    }chanaleView[TotalSizeChanalesView];
    Ui::Channels *ui;
    ChannelModel *m_channelModel;
    ChannelModel *m_channelMathModel;
    QVector<VarChannel*> *m_channels;
    QVector<VarChannel*> *m_channelsMath;
    QSortFilterProxyModel *chanaleProxyModel;
    QSortFilterProxyModel *chanaleMathProxyModel;
//    ChanaleItemDelegate iteamDeclarater;
    //sequnce sent color for new gruph
    QList<QColor> colorSetSequese;
    int curentColorSet;
    int curentDotStyle;
};

#endif // CHANNELS_H
