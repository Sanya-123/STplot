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

            QString displayName;
            QtnPropertySet* parentProp = findParentProperty(name, &displayName, propertySets);

            switch (var.type()) {
                case QVariant::Color:
                {
                    QtnPropertyQColor* prop = qtnCreateProperty<QtnPropertyQColor>(parentProp);
                    prop->setValue(var.value<QColor>());
                    propBase = prop;
                    break;
                }
                default:
                {
                    QtnPropertyQString* prop = qtnCreateProperty<QtnPropertyQString>(parentProp);
                    prop->setValue(QString("unnoun type"));
                    propBase = prop;
                    break;
                }
            }

            if(propBase != nullptr)
            {
                propBase->setName(name);
                propBase->setDisplayName(displayName);
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

QtnPropertySet * PlotSettingsViever::findParentProperty(QString propertyName, QString *displayName, QtnPropertySet *propertySets)
{
    QList<QString> tree = propertyName.split('.');

    QtnPropertySet *findProperty = propertySets;
//    QtnPropertyBase *res = nullptr;

    if(tree.size() == 0)
        tree.append(propertyName);

    for(int i = 0; i < tree.size(); i++)
    {
        if(i != (tree.size() - 1))
        {
            QList<QtnPropertyBase *> list = findProperty->findChildProperties(tree[i]);
            if(list.size() != 0)//it should be 1
            {
                findProperty = (QtnPropertySet *)list[0];
            }
            else//else mean than no current property
            {
                findProperty = qtnCreateProperty<QtnPropertySet>(findProperty);
                findProperty->setName(tree[i]);
            }

            if(list.size() > 1)
            {
                qDebug() << "Warninig: value" << tree[i] << "more than 1";
            }
        }
        else
        {
//            QList<QtnPropertyBase *> list = findProperty->findChildProperties(propertyName);
//            if(list.size() != 0)//it should be 1
//            {
//                res = list[0];
//            }
//            else
//            {
//                res = qtnCreateProperty<QtnPropertyFloat>(findProperty);
//                res->setDisplayName(tree[i]);
//                res->setName(tree[i]);
//            }

//            if(list.size() > 1)
//            {
//                qDebug() << "Warninig: value" << tree[i] << "more than 1";
//            }
        }
    }

    *displayName = tree.last();

    return findProperty;
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
