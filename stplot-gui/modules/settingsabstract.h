#ifndef SETTINGSABSTRACT_G
#define SETTINGSABSTRACT_G
#include <QObject>
#include <QString>
#include <QSettings>
#include <QMap>
/**
 * @brief The SettingsAbstract class - abstract setting class,
 */
class SettingsAbstract : public QObject
{
    Q_OBJECT
public:
    //int constructor I should fill mapSettings
    SettingsAbstract (QObject *parent = nullptr) : QObject(parent){mapSettings = mapSettingsDefauold;}
    QMap<QString, QVariant> getSettingsMap() {return mapSettings;}

public slots:
    /**
     * @brief setSettings - metod copy form setting to mapSettings
     * @param settings
     */
    virtual void setSettings(SettingsAbstract *settings) {setSettings(settings->getSettingsMap());}
    virtual void setSettings(QMap<QString, QVariant> nemMap)
    {
        QList<QString> names = nemMap.keys();
        foreach (QString name, names) {
            setValues(name, nemMap[name]);
        }
    }
    /**
     * @brief setValues - set value for setings
     * @param name - name setting
     * @param val - value setting
     */
    virtual void setValues(QString name, QVariant val)
    {
        if(mapSettings.contains(name))
        {
            if(mapSettings[name].type() == val.type())
            {
                mapSettings[name] = val;
                emit settingsUpdated();
            }
        }
    }

    virtual void restoreDefoultSetings() {mapSettings = mapSettingsDefauold; emit settingsUpdated();}


signals:
#ifdef WIN32
    virtual void settingsUpdated() = 0;
#else
    void settingsUpdated();
#endif

protected:
    //map settings mname and values
    QMap<QString, QVariant> mapSettingsDefauold;
    //    friend class PlotWidgetAbstract;
    //    friend class PlotWidgetInterfacePlugin;
private:
    QMap<QString, QVariant> mapSettings;
};

#endif // SETTINGSABSTRACT_G
