#include "viewmanager.h"
#include "ui_viewmanager.h"
#include <QDebug>
#include <QDir>
#include <QPluginLoader>

ViewManager::ViewManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewManager)
{
    ui->setupUi(this);
}

ViewManager::~ViewManager()
{
    delete ui;
}

void ViewManager::saveSettings(QSettings *settings)
{

}

void ViewManager::restoreSettings(QSettings *settings)
{

}

void ViewManager::loadPlugin()
{
    PlotWidgetInterfacePlugin *_interface;
    QDir pluginsDir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif

    if(!pluginsDir.cd("./plugins"))
        pluginsDir.cd("../plugins");

    qDebug() << "Plugin path:" << pluginsDir.absolutePath();

    const QStringList entries = pluginsDir.entryList(QDir::Files);//get list plugins files

    for (const QString &fileName : entries) {
        qDebug() << "Try load plugin:" << fileName;
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));//load file as plugin
        QObject *plugin = pluginLoader.instance();//get object plugin
        if (plugin) {
            _interface = qobject_cast<PlotWidgetInterfacePlugin *>(plugin);//check type plugin
            if (_interface)//!0 if type correct
            {
                VersionUnion _version;
                _version.version32 = _interface->getVersion();
                qDebug() << "OK plugin:" << _interface->getName() <<
                            QString::asprintf(";v%d.%d.%d", _version.versionS.major, _version.versionS.minor, _version.versionS.build);

                _version.versionS.build = 0;//do note abalize build version just major and minor

                //check version
                if(_version.version32 >= MINIMUM_PLUGIN_PLOT_HEADER_VERSION)
                {
                    if(_version.version32 <= PLOT_INTERFACE_HEADER_VERSION)//maximum avaleble version
                        pluginsPlot.append(_interface);
                    else
                    {
                        qDebug() << "App is too old for curent plugin please rebuild it";
                        pluginLoader.unload();
                    }
                }
                else
                {
                    qDebug() << "Plugin is too old please rebuild it";
                    pluginLoader.unload();
                }
            }
            else
            {
                qDebug() << "unload plugin:" << fileName;
                pluginLoader.unload();
            }

        }
    }
}

void ViewManager::on_tableWidget_cellChanged(int row, int column)
{
    qDebug() << "change:" << row << column;
}

void ViewManager::on_pushButton_addView_clicked()
{

}

void ViewManager::on_pushButton_deleteViewes_clicked()
{

}

