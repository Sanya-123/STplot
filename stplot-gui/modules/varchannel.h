#ifndef VARCHANNEL_H
#define VARCHANNEL_H

#include <QObject>
#include <QColor>
#include <QString>
#include <vector>
#include <chrono>

#include "varloc.h"
//#include "qcustomplot.h"

using time_stamp = std::chrono::time_point<std::chrono::system_clock,
                                           std::chrono::microseconds>;

typedef struct {
    double value;
    time_stamp time;
}VarValue;

class VarChannel : public QObject
{
    Q_OBJECT
public:
    VarChannel(varloc_node_t* node);
    ~VarChannel();
    void push_value(float value);
    QString name();
    float value();
    bool has_var_node(varloc_node_t* node);
    uint32_t addres();

    QVector<bool> getPlotList();
    int getTotalSizePlot();
    void setTotalSizePlot(int size);
    void removePlot(int numberPlot);
    bool isEnableOnPlot(int numberPlot);
    void setEnableOnPlot(int numberPlot, bool en);

    VarValue getValue(int numberElement);
    QVector<VarValue> getBuffer();



private:
    float                   m_value;
    QString                 m_name;
    varloc_node_t*          m_var_node;
    QVector<VarValue>       m_buffer;
    //list for each plot gruph
    QVector<bool>           m_plotListl;

    //graph styles
    QColor                  m_lineColor;
//    QCPGraph::LineStyle     m_lineStyle;
//    QCPScatterStyle::ScatterShape m_dotStyle;
    int                     m_lineStyle;
    int                     m_dotStyle;

signals:
    void changePlotColor();
    void changePlotLineStyle();
    void changePlotDotStyle();
    void changeName();
    void updatePlot();
};

#endif // VARCHANNEL_H
