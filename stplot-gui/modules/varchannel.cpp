#include "varchannel.h"

using namespace std::chrono;

VarChannel::VarChannel(varloc_node_t* node, QColor lineColor, int dotStyle) : tmpDesc(0), m_lineColor(lineColor)
{
    if (node != NULL){
        m_var_node = node;
        m_name = QString(node->name);
        varloc_node_t * parent = var_node_get_parent(node);
        while (parent != NULL){
            m_name.prepend(".");
            m_name.prepend(parent->name);
            parent = var_node_get_parent(parent);
        }
//        m_buffer.reserve(100);
    }

    m_dotStyle = MAX_DEFAOULT_DOT_STYLE;
    m_lineStyle = MAX_DEFAOULT_LINE_STYLE;

    setDotStyle(dotStyle);
}

VarChannel::~VarChannel(){

}

void VarChannel::push_value(float value){
    VarValue var = {
        .value = value,
//        .time = time_point_cast<microseconds>(system_clock::now()),
        .qtime = QTime::currentTime(),
    };
//    m_buffer.push_back(var);
    m_buffer.append(var);

    tmpDesc++;
    if(tmpDesc == 100)
    {
        tmpDesc = 0;
        emit updatePlot();
    }
}

bool VarChannel::has_var_node(varloc_node_t* node){
    return (m_var_node == node);
}

uint32_t VarChannel::addres()
{
    return m_var_node->address.base;
}

QString VarChannel::name(){
    return m_name;
}

float VarChannel::value(){
    return m_value;
}

QVector<bool> VarChannel::getPlotList()
{
    return m_plotListl;
}

int VarChannel::getTotalSizePlot()
{
    return m_plotListl.size();
}

void VarChannel::setTotalSizePlot(int size)
{
    m_plotListl.resize(size);
}

void VarChannel::removePlot(int numberPlot)
{
    if(m_plotListl.size() > numberPlot)
    {
        m_plotListl.removeAt(numberPlot);
    }
}

bool VarChannel::isEnableOnPlot(int numberPlot)
{
    if(m_plotListl.size() > numberPlot)
    {
        return m_plotListl[numberPlot];
    }
    return false;
}

void VarChannel::setEnableOnPlot(int numberPlot, bool en)
{
    if(m_plotListl.size() > numberPlot)
    {
        m_plotListl[numberPlot] = en;
    }
}

VarValue VarChannel::getValue(int numberElement)
{
    if(numberElement < m_buffer.size() && numberElement >= 0)
        return m_buffer[numberElement];


    return (VarValue){0, QTime::currentTime()};
}

QVector<VarValue> VarChannel::getBuffer()
{

    return m_buffer;
}

int VarChannel::getBufferSize()
{
    return m_buffer.size();
}

int VarChannel::dotStyle() const
{
    return m_dotStyle;
}

void VarChannel::setDotStyle(int newDotStyle)
{
    if(newDotStyle < 0)
        newDotStyle = 0;
    if(newDotStyle >= MAX_NUMBER_DOT_STYLE)
        newDotStyle = MAX_NUMBER_DOT_STYLE - 1;

    m_dotStyle = newDotStyle;

    emit changePlotDotStyle();
}

int VarChannel::lineStyle() const
{
    return m_lineStyle;
}

void VarChannel::setLineStyle(int newLineStyle)
{
    if(newLineStyle < 0)
        newLineStyle = 0;
    if(newLineStyle >= MAX_NUMBER_LINE_STYLE)
        newLineStyle = MAX_NUMBER_LINE_STYLE - 1;

    m_lineStyle = newLineStyle;

    emit changePlotLineStyle();
}

QColor VarChannel::lineColor() const
{
    return m_lineColor;
}

void VarChannel::setLineColor(const QColor &newLineColor)
{
    m_lineColor = newLineColor;
    emit changePlotColor();
}
