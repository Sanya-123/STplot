#include "plotsettingsviever.h"
#include "QtnProperty/GUI/PropertyQColor.h"
#include "QtnProperty/GUI/PropertyQPen.h"
#include <QLayout>
#include <QDebug>

PlotSettingsViever::PlotSettingsViever(PlotSettingsAbstract *settings, QWidget *parent)
    : QDialog{parent}
{
    propertySets = new QtnPropertySet(this);
    propWidget.setPropertySet(propertySets);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(&propWidget);

    this->setLayout(layout);

    if(settings != nullptr)
    {
        QMap<QString, QVariant> map = settings->getSettingsMap();
        QList<QString> names = map.keys();
        foreach (QString name, names) {
            QtnPropertyBase *propBase = nullptr;
            QVariant var = map[name];
            switch (var.type()) {
                case QVariant::Color:
                {
                    QtnPropertyQColor* prop = qtnCreateProperty<QtnPropertyQColor>(propertySets);
                    prop->setValue(var.value<QColor>());
                    propBase = prop;
                    break;
                }
                default:
                {
                    QtnPropertyQString* prop = qtnCreateProperty<QtnPropertyQString>(propertySets);
                    prop->setValue(QString("unnoun type"));
                    propBase = prop;
                    break;
                }
            }

            if(propBase != nullptr)
            {
                propBase->setName(name);
                connect(propBase, SIGNAL(propertyDidChange(QtnPropertyChangeReason)), this, SLOT(changeValue(QtnPropertyChangeReason)));
            }
        }
    }

    this->settings = settings;

    setModal(true);
}

PlotSettingsViever::~PlotSettingsViever()
{

}

void PlotSettingsViever::changeValue(QtnPropertyChangeReason reson)
{
    //after edit reason is 0x0401
    if(reson & QtnPropertyChangeReasonEdit)
    {
        QtnPropertyBase *propBase = (QtnPropertyBase *)sender();

//        qDebug() << typeid(*propBase).name();

        //get type walyes
        if(typeid(*propBase) == typeid(QtnPropertyQColor))
        {
//            qDebug() << "QtnPropertyQColor";
            settings->setValues(propBase->name(), (QColor)((QtnPropertyQColor*)propBase)->value());
        }

        if(typeid(*propBase) == typeid(QtnPropertyQString))
        {
            qDebug() << "QtnPropertyQString";
        }

//        settings->
    }

}
