#ifndef ELFREADER_H
#define ELFREADER_H

#include <QObject>
#include "varreaderinterface.h"

class ElfReader : public QObject, VarReadInterfacePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.S.debuger.ElfReader" FILE "pluginElfReader.json")
    Q_INTERFACES(VarReadInterfacePlugin)

public:
    explicit ElfReader(QObject *parent = nullptr);

    QString getName() { return QString("ELF file"); }

    QString getFileExtensions() {return QString("*.elf");}

    QIODevice::OpenModeFlag allowMode() {return QIODevice::ReadOnly;}

    varloc_node_t *readTree(QString fileName);

    int saveTree(varloc_node_t* tree, QString fileName) {return -1;}

signals:

};

#endif // ELFREADER_H
