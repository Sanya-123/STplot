#include "settingsdialog.h"
#include "QtnProperty/GUI/PropertyQColor.h"
#include "QtnProperty/GUI/PropertyQPen.h"
#include <QLayout>
#include <QDebug>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>

SettingsDialog::SettingsDialog(SettingsAbstract *settings, QWidget *parent)
    : QDialog{parent},
    settingsTemplates(QDir::currentPath() + "/" + VIEW_MODE_FILE_NAME, QSettings::IniFormat)
{
    propertySets = new QtnPropertySet(this);
    propWidget.setPropertySet(propertySets);

    loadSettingsName = new QComboBox(this);
    saveSettingsName = new QLineEdit(this);
    loadSettingsButton = new QPushButton("load view", this);
    saveSettingsButton = new QPushButton("save view", this);

    connect(loadSettingsButton, SIGNAL(clicked(bool)), this, SLOT(loadSettings()));
    connect(saveSettingsButton, SIGNAL(clicked(bool)), this, SLOT(saveSettings()));
    connect(loadSettingsName, SIGNAL(currentTextChanged(QString)), saveSettingsName, SLOT(setText(QString)));

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(&propWidget, 0, 0, 1, 2);
    layout->addWidget(loadSettingsName, 1, 0);
    layout->addWidget(loadSettingsButton, 1, 1);
    layout->addWidget(saveSettingsName, 2, 0);
    layout->addWidget(saveSettingsButton, 2, 1);

    loadSettingsName->addItems(settingsTemplates.allKeys());

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
                case QVariant::Bool:
                {
                    QtnPropertyBool* prop = qtnCreateProperty<QtnPropertyBool>(parentProp);
                    prop->setValue(var.value<bool>());
                    propBase = prop;
                    break;
                }
                case QVariant::Double:
                {
                    QtnPropertyDouble* prop = qtnCreateProperty<QtnPropertyDouble>(parentProp);
                    prop->setValue(var.value<double>());
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
                QString propertyName = name;
                propertyName.replace(".", "_");//replase it for allow save it
                mapPpopNameToParName[propertyName] = name;
                propBase->setDisplayName(displayName);
                propBase->setName(propertyName);
                connect(propBase, SIGNAL(propertyDidChange(QtnPropertyChangeReason)), this, SLOT(changeValue(QtnPropertyChangeReason)));
            }
        }
    }

    this->settings = settings;

    setModal(true);
}

SettingsDialog::~SettingsDialog()
{

}

QtnPropertySet * SettingsDialog::findParentProperty(QString propertyName, QString *displayName, QtnPropertySet *propertySets)
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

void SettingsDialog::changeValue(QtnPropertyChangeReason reson)
{
    //after edit reason is 0x0401
    if(reson & QtnPropertyChangeReasonEdit)
    {
        QtnPropertyBase *propBase = (QtnPropertyBase *)sender();

//        qDebug() << typeid(*propBase).name();

        QString parName = mapPpopNameToParName[propBase->name()];

        //get type walyes
        if(typeid(*propBase) == typeid(QtnPropertyQColor))
        {
//            qDebug() << "QtnPropertyQColor";
            settings->setValues(parName, (QColor)((QtnPropertyQColor*)propBase)->value());
        }

        if(typeid(*propBase) == typeid(QtnPropertyBool))
        {
            settings->setValues(parName, (bool)((QtnPropertyBool*)propBase)->value());
        }

        if(typeid(*propBase) == typeid(QtnPropertyDouble))
        {
            settings->setValues(parName, (double)((QtnPropertyDouble*)propBase)->value());
        }

        if(typeid(*propBase) == typeid(QtnPropertyQString))
        {
//            qDebug() << "QtnPropertyQString";
        }

//        settings->
    }

}

void SettingsDialog::saveSettings()
{
    QString setteingsname = saveSettingsName->text();

    QJsonObject jsonProp;
    if(!propertySets->toJson(jsonProp))
    {
        qCritical() << "Couldn't convert property for save";
        return;
    }

    if(!settingsTemplates.contains(setteingsname))
        loadSettingsName->addItem(setteingsname);

    settingsTemplates.setValue(setteingsname, QJsonDocument(jsonProp).toBinaryData());
}

void SettingsDialog::loadSettings()
{
    QString setteingsname = loadSettingsName->currentText();

    if(settingsTemplates.contains(setteingsname))
    {
        QJsonDocument jsonDoc = QJsonDocument::fromBinaryData(settingsTemplates.value(setteingsname).toByteArray());
        if(!jsonDoc.isObject() || jsonDoc.isEmpty() || jsonDoc.isNull())//check file
        {
            qCritical() << "Couldn't read curent config";
        }
        else
        {
            if(!propertySets->fromJson(jsonDoc.object(), QtnPropertyChangeReasonEdit))
            {
                qCritical() << "Couldn't convert property for load";
            }

//            settings->
        }
    }
}
