#ifndef VARCHANNEL_H
#define VARCHANNEL_H

#include <QObject>
#include <QColor>
#include <QString>
#include <vector>
#include <chrono>
#include <QTime>

#include "varloc.h"
//#include "qcustomplot.h"

//using time_stamp = std::chrono::time_point<std::chrono::system_clock,
//                                           std::chrono::microseconds>;
#define MAX_NUMBER_DOT_STYLE                16
#define MAX_NUMBER_LINE_STYLE               6

#define MAX_DEFAOULT_DOT_STYLE              1
#define MAX_DEFAOULT_LINE_STYLE             1

typedef struct {
    double value;
    QTime qtime;
}VarValue;

class VarChannel : public QObject
{//TODO add slot and signal write data for write it throu plagins plot and then in read manager connet signals with request write device
    Q_OBJECT
public:
    VarChannel(varloc_node_t* node, QColor lineColor = Qt::green, int dotStyle = MAX_DEFAOULT_DOT_STYLE);
    VarChannel(varloc_location_t location, QString name, QColor lineColor = Qt::green, int dotStyle = MAX_DEFAOULT_DOT_STYLE);
    ~VarChannel();
    QString getName();
    float getValue();
    varloc_location_t getLocation();
    uint32_t getMask();
    float decode_value(uint32_t value);
    bool hasLocation(varloc_location_t loc);

    static QString getFullNmaeNode(varloc_node_t* node);

    QVector<bool> getPlotList();
    int getTotalSizePlot();
    void setTotalSizePlot(int size);
    void removePlot(int numberPlot);
    bool isEnableOnPlot(int numberPlot);
    void setEnableOnPlot(int numberPlot, bool en);

    VarValue getValue(int numberElement);
    QVector<VarValue> getBuffer();
    int getBufferSize();
    int dotStyle() const;
    void setDotStyle(int newDotStyle);
    void setLocation(varloc_location_t loc);
    void clearLocation();
    int lineStyle() const;
    void setLineStyle(int newLineStyle);

    QColor lineColor() const;
    void setLineColor(const QColor &newLineColor);

    void setLineWidth(unsigned int newWidth);
    unsigned int lineWidth() const;

    QString displayName() const;
    void setDisplayName(const QString &newDisplayName);

public slots:
    void pushValue(float value, QTime record_time);
    void pushValueRaw(uint32_t value);
    void pushValueRawWithTime(uint32_t value, QDateTime date_time);
    void selectCurentPlot();

private:
    float                   m_value;
    QString                 m_name;
    QString                 m_displayName;
    varloc_location_t       m_location;
    uint32_t                m_mask;
//    varloc_node_t*          m_var_node;
    QVector<VarValue>       m_buffer;
    //list for each plot gruph
    QVector<bool>           m_plotListl;

    //graph styles
    QColor                  m_lineColor;
    unsigned int            m_lineWidth;
//    QCPGraph::LineStyle     m_lineStyle;
//    QCPScatterStyle::ScatterShape m_dotStyle;
    int                     m_dotStyle;
    int                     m_lineStyle;

    int tmpDesc;

signals:
    void changePlotColor();
    void changePlotLineWidth();
    void changePlotLineStyle();
    void changePlotDotStyle();
    void changeDisplayName();
    void updatePlot();
    void selectPlot();
};

#endif // VARCHANNEL_H
