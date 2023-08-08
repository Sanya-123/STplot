#include "varchannel.h"

using namespace std::chrono;

VarChannel::VarChannel(varloc_node_t* node)
{
    if (node != NULL){
        m_var_node = node;
        m_name = new QString(node->name);
        varloc_node_t * parent = var_node_get_parent(node);
        while (parent != NULL){
            m_name->prepend(".");
            m_name->prepend(parent->name);
            parent = var_node_get_parent(parent);
        }
        m_buffer.reserve(100);
    }
}

VarChannel::~VarChannel(){
    delete m_name;
}

void VarChannel::push_value(float value){
    VarValue var = {
        .value = value,
        .time = time_point_cast<microseconds>(system_clock::now()),
    };
    m_buffer.push_back(var);
}

bool VarChannel::has_var_node(varloc_node_t* node){
    return (m_var_node == node);
}

QString* VarChannel::name(){
    return m_name;
}

float VarChannel::value(){
    return m_value;
}
