#ifndef VARCHANNEL_H
#define VARCHANNEL_H

#include <QString>
#include <vector>
#include <chrono>

#include "varloc.h"
#include "plotinterface.h"


class VarChannel
{
public:
    VarChannel(varloc_node_t* node);
    ~VarChannel();
    void push_value(float value);
    QString* name();
    float value();
    bool has_var_node(varloc_node_t* node);
private:
    float                   m_value;
    QString*                m_name;
    varloc_node_t*          m_var_node;
    std::vector<VarValue>   m_buffer;
};

#endif // VARCHANNEL_H
