#include "varview.h"

VarView::VarView(char* new_name)
{
    name = QString(new_name);
}


void VarView::add_channel(VarChannel *channel)
{
    m_channels.append(channel);
}

void VarView::remove_channel(VarChannel *channel)
{
    m_channels.removeOne(channel);
}
