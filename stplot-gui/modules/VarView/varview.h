#ifndef VARVIEW_H
#define VARVIEW_H

#include "varchannel.h"
#include <QString>
#include <QVector>
class VarView
{
public:
    VarView(char *new_name);
    void add_channel(VarChannel *channel);
    void remove_channel(VarChannel *channel);

    QString                     name;

private:
    QVector<VarChannel*>    m_channels;
};
#endif // VARVIEW_H
