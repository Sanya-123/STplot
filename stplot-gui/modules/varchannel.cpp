#include "varchannel.h"

using namespace std::chrono;

VarChannel::VarChannel(varloc_node_t* node)
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
}

VarChannel::~VarChannel(){

}

void VarChannel::push_value(float value){
    VarValue var = {
        .value = value,
        .time = time_point_cast<microseconds>(system_clock::now()),
    };
//    m_buffer.push_back(var);
    m_buffer.append(var);
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


    return (VarValue){0, time_point_cast<microseconds>(system_clock::now())};
}

QVector<VarValue> VarChannel::getBuffer()
{

    return m_buffer;
}
