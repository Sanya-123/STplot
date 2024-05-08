#ifndef STM_STUDION_FILE_H
#define STM_STUDION_FILE_H

#include <QObject>
#include "readwritedevice.h"
#include "stmstudiofiledevice.h"

class StmStudioFile : public QObject, SaveDeviceInterfacePlugin/*, ReadDeviceInterfacePlugin */
{//ReadDeviceInterfacePlugin it is note need becouse eache save devisa also erad device
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.S.debuger.StmStuduioFileDevice" FILE "pluginStmStuduioFileDevice.json")
    Q_INTERFACES(SaveDeviceInterfacePlugin /*ReadDeviceInterfacePlugin*/)

public:
    StmStudioFile(QObject *parent = nullptr) : QObject(parent) {}
//    ReadDeviceObject* createReadDeviceObject() {return &device;}
    SaveDeviceObject* createWriteDeviceObject() {return &device;}
    QString getName() {return "STLinkFile";}

private:
    STMstudioFileDevice device;
};

#endif // STM_STUDION_FILE_H
