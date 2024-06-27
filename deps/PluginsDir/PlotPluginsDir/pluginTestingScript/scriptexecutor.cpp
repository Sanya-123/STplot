#include "scriptexecutor.h"
#include <QThread>
#include <QDebug>

ScriptExecutor::ScriptExecutor(QObject *parent)
    : QObject{parent}
{
    {//delay function
        auto delay = [] (QScriptContext *context, QScriptEngine *engine) -> QScriptValue
        {
            Q_UNUSED(engine);
            QScriptValue delayTime = context->argument(0);
            QThread::msleep(delayTime.toInt32());
            return QScriptValue(0.0);
        };
        QScriptEngine::FunctionSignature delayLambdaPtr = delay;
        QScriptValue fun = myEngine.newFunction(delayLambdaPtr);
        myEngine.globalObject().setProperty("delay", fun);
    }

    //set pointer to parrat class
    QScriptValue thisObject = myEngine.newQObject(this);
    myEngine.globalObject().setProperty("main", thisObject);

    //set object list object
    QScriptValue listObject = myEngine.newQObject(&listVaribelsObject);
    myEngine.globalObject().setProperty("mapVar", listObject);

    {
        //get value function
        auto getValue = [] (QScriptContext *context, QScriptEngine *engine) -> QScriptValue
        {
            ScriptExecutorListVaribels *listVar = (ScriptExecutorListVaribels*)engine->globalObject().property("mapVar").toQObject();
            QScriptValue chanaleName = context->argument(0);
            if(listVar->contains(chanaleName.toString()))
                return QScriptValue(1.0*listVar->value(chanaleName.toString())->getValue());
            return QScriptValue(0.0);
        };
        QScriptEngine::FunctionSignature getValueLambdaPtr = getValue;
        QScriptValue funGet = myEngine.newFunction(getValueLambdaPtr);
        myEngine.globalObject().setProperty("getValue", funGet);
//        myEngine.globalObject().setProperty("g", funGet);

        //set values
        auto setValue = [] (QScriptContext *context, QScriptEngine *engine) -> QScriptValue
        {
            ScriptExecutorListVaribels *listVar = (ScriptExecutorListVaribels*)engine->globalObject().property("mapVar").toQObject();
            QScriptValue chanaleName = context->argument(0);
            QScriptValue chanaleValue = context->argument(1);
            if(listVar->contains(chanaleName.toString()))
            {
                listVar->value(chanaleName.toString())->writeValues(chanaleValue.toNumber());
            }
            return QScriptValue(0.0);
        };
        QScriptEngine::FunctionSignature setValueLambdaPtr = setValue;
        QScriptValue funSet = myEngine.newFunction(setValueLambdaPtr);
        myEngine.globalObject().setProperty("setValue", funSet);
//        myEngine.globalObject().setProperty("s", funSet);
    }
}

void ScriptExecutor::putMessadge(QString msg)
{
    emit printMessadge(msg);
}

void ScriptExecutor::executeScript()
{
    myEngine.evaluate(this->script);

    if(myEngine.hasUncaughtException())
    {//not each line is executed
        //print error
        putMessadge(tr("Couldn't execute line:") + QString::number(myEngine.uncaughtExceptionLineNumber()));
    }

    emit finishExecScript();
}

QScriptSyntaxCheckResult ScriptExecutor::setScript(QString script)
{
    this->script = script;
    return myEngine.checkSyntax(script);
}

void ScriptExecutor::setVarChanales(QVector<VarChannel *> varChanales)
{
    listVaribelsObject.clear();
    mapVar.clear();
    foreach (VarChannel *var, varChanales) {
        mapVar[var->displayName()] = var;
        listVaribelsObject[var->displayName()] = var;
    }
}
