#include "elfreader.h"
#ifndef Q_OS_WINDOWS
#include "varloc.h"
#endif

ElfReader::ElfReader(QObject *parent) : QObject(parent)
{

}

varloc_node_t *ElfReader::readTree(QString fileName)
{
#ifndef Q_OS_WINDOWS
    return varloc_open_elf(fileName.toLocal8Bit().data());
#else
    return nullptr;
#endif
}
