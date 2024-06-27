#ifndef SCRIPTEXECUTOR_H
#define SCRIPTEXECUTOR_H

#include <QObject>
#include <QScriptEngine>
#include "varchannel.h"

/**
 * @brief The ScriptExecutorListVaribels class - objet that contain map with varibels
 * make a object becouse I need acee to from scrit engine but I coult sen to script engine just object
 */
class ScriptExecutorListVaribels : public QObject, public QMap<QString, VarChannel*>
{
    Q_OBJECT
public:
    ScriptExecutorListVaribels(QObject *parent = nullptr)  : QObject(parent) {}
};

class ScriptExecutor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString msg WRITE putMessadge)
public:
    explicit ScriptExecutor(QObject *parent = nullptr);

    //use for print messadge throw script
    void putMessadge(QString msg);

public slots:
    void executeScript();
    QScriptSyntaxCheckResult setScript(QString script);
    void setVarChanales(QVector<VarChannel*> varChanales);

signals:
    void printMessadge(QString msg);
    void finishExecScript();

private:
    QScriptEngine myEngine;
    QString script;
//    QVector<VarChannel*> *mapPlots;
    QMap<QString, VarChannel*> mapVar;
    ScriptExecutorListVaribels listVaribelsObject;
};

#endif // SCRIPTEXECUTOR_H
