#include "varchannel.h"
#include <QtMath>

using namespace std::chrono;

VarChannel::VarChannel(varloc_node_t* node, QColor lineColor, int dotStyle) :
    m_lineWidth(1), tmpDesc(0), m_lineColor(lineColor)
{
    if (node != NULL){
        m_location = var_node_get_load_location(node);
        m_name = getFullNmaeNode(node);
//        m_buffer.reserve(100);
    }

    m_dotStyle = MAX_DEFAOULT_DOT_STYLE;
    m_lineStyle = MAX_DEFAOULT_LINE_STYLE;
    m_displayName = m_name;

    setDotStyle(dotStyle);

    m_mask = /*pow(2, m_location.address.size_bits)*/(1 << m_location.address.size_bits) - 1;
    if(m_location.address.size_bits == 32)
        m_mask = 0xFFFFFFFF;

    m_mask = m_mask << m_location.address.offset_bits;
}

VarChannel::VarChannel(varloc_location_t location, QString name, QColor lineColor, int dotStyle) :
    m_lineWidth(1), tmpDesc(0), m_lineColor(lineColor)
{
    m_location = location;
    m_name = name;
    m_displayName = m_name;

    m_dotStyle = MAX_DEFAOULT_DOT_STYLE;
    m_lineStyle = MAX_DEFAOULT_LINE_STYLE;

    setDotStyle(dotStyle);

    m_mask = /*pow(2, m_location.address.size_bits)*/(1 << m_location.address.size_bits) - 1;
    if(m_location.address.size_bits == 32)
        m_mask = 0xFFFFFFFF;

    m_mask = m_mask << m_location.address.offset_bits;
}

VarChannel::~VarChannel(){

}

void VarChannel::pushValue(float value){
    m_value = value;
    VarValue var = {
        .value = value,
//        .time = time_point_cast<microseconds>(system_clock::now()),
        .qtime = QTime::currentTime(),
    };
//    m_buffer.push_back(var);
    m_buffer.append(var);
    tmpDesc++;
    if(tmpDesc == 10)
    {
        tmpDesc = 0;
        emit updatePlot();
    }
}

void VarChannel::pushValueRaw(uint32_t value){

    union {
        _Float32    _f;
        uint8_t     _u8;
        int8_t      _i8;
        uint16_t    _u16;
        int16_t     _i16;
        uint32_t    _u32;
        int32_t     _i32;
    }combiner;
    combiner._u32 = (value & m_mask) >> m_location.address.offset_bits;
    if(m_location.address.size_bits <= 8){
        if (m_location.type == VARLOC_SIGNED){
            m_value = combiner._i8;
        } else{
            m_value = combiner._u8;
        }
    }
    else if(m_location.address.size_bits <= 16){
        if (m_location.type == VARLOC_SIGNED){
            m_value = combiner._i16;
        } else{
            m_value = combiner._u16;
        }
    }
    else {
        if (m_location.type == VARLOC_SIGNED){
            m_value = combiner._i32;
        }
        else if (m_location.type == VARLOC_FLOAT){
            m_value = combiner._f;
        } else{
            m_value = combiner._u32;
        }
    }
    pushValue(m_value);
}

void VarChannel::selectCurentPlot()
{
    emit selectPlot();
}

QString VarChannel::displayName() const
{
    return m_displayName;
}

void VarChannel::setDisplayName(const QString &newDisplayName)
{
    m_displayName = newDisplayName;
    emit changeDisplayName();
}

varloc_location_t VarChannel::getLocation()
{
    return m_location;
}

uint32_t VarChannel::getMask()
{
    return m_mask;
}

bool VarChannel::hasLocation(varloc_location_t loc){
    if ((loc.address.base == this->m_location.address.base)
    && (loc.address.offset_bits == this->m_location.address.offset_bits)
    && (loc.address.size_bits == this->m_location.address.size_bits)
    ){
        return true;
    }
    return false;

}

QString VarChannel::getFullNmaeNode(varloc_node_t *node)
{
    if(node == nullptr)
        return QString();


    QString name = QString(node->name);
    varloc_node_t * parent = var_node_get_parent(node);
    while (parent != NULL){
        if (!parent->is_anon){
            name.prepend(".");
            name.prepend(parent->name);
        }
        parent = var_node_get_parent(parent);
    }

    return name;
}

QString VarChannel::getName(){
    return m_name;
}

float VarChannel::getValue(){
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


    return (VarValue){0, QTime::currentTime()};//NOTE could be paroblem in save files
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

unsigned int VarChannel::lineWidth() const
{
    return m_lineWidth;
}

void VarChannel::setLineWidth(unsigned int newWidth)
{
    m_lineWidth = newWidth;
    emit changePlotLineWidth();
}
