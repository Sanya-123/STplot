#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "varchannel.h"
#include "comboboxdelegate.h"

class ChannelModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ChannelModel(QVector<VarChannel*> *channels, bool mathChanale = false, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    static ComboBoxDelegate* makeIteamLineStye(QObject *parent = nullptr);
    static ComboBoxDelegate* makeIteamDotStye(QObject *parent = nullptr);
    static ComboBoxDelegate* makeIteamLineWidth(QObject *parent = nullptr);
    static QStringList getDotStyle();
    static QStringList getLineStyle();
    static QStringList getLineWidth();

    QList<QString> getGraphNames() const;

public slots:
    void addPlot();
    void deletePlot(int number);
    void setPlotName(int number, QString name);
    void selectChanale(QModelIndex index);
    void setEnablePlot(VarChannel* chanale, int numGruph, bool en);

signals:
    void updateViewport();
    void changeEnablePlo(VarChannel* var, int numPlot, bool en);

private:
    QVector<VarChannel*> *m_channels;
    int numberGraph;
    QList<QString> graphNames;

    QStringList dotStyles;
    QStringList lineStyles;
    QStringList lineWidths;

    bool isMathChanale;

};

#endif // CHANNELMODEL_H
