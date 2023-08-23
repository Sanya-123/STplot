#ifndef VARCHANNEL_H
#define VARCHANNEL_H

#include <QString>
#include <vector>
#include <chrono>

#include "varloc.h"
#include "plotinterface.h"
#include "qcustomplot.h"


class VarChannel : public QObject
{
    Q_OBJECT
public:
    VarChannel(varloc_node_t* node);
    ~VarChannel();
    void push_value(float value);
    QString* name();
    float value();
    bool has_var_node(varloc_node_t* node);
    uint32_t addres();

    QVector<bool> getPlotList();
    int getTotalSizePlot();
    void setTotalSizePlot(int size);
    void removePlot(int numberPlot);
    bool isEnableOnPlot(int numberPlot);
    void setEnableOnPlot(int numberPlot, bool en);



private:
    float                   m_value;
    QString*                m_name;
    varloc_node_t*          m_var_node;
    std::vector<VarValue>   m_buffer;
    //list for each plot gruph
    QVector<bool>           m_plotListl;

    //graph styles
    QColor                  m_lineColor;
    QCPGraph::LineStyle     m_lineStyle;
    QCPScatterStyle::ScatterShape m_dotStyle;

signals:
    void changePlotColor();
    void changePlotLineStyle();
    void changePlotDotStyle();
    void changeName();
    void updatePlot();
};

#endif // VARCHANNEL_H
