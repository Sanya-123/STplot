#include "stplotwindow.h"
#include "debugerwindow.h"

#include <QApplication>

DebugerWindow *debuger = nullptr;

#define END_COLOR           "\033[0m"
#define RED_BEGIN           "\033[31m"
#define GREE_BEGIN          "\033[32m"
#define YELLOW_BEGIN        "\033[33m"
#define BLUE_BEGIN          "\033[34m"
#define PURPUR_BEGIN        "\033[35m"
#define BERUZ_BEGIN         "\033[36m"
#define WHITE_BEGIN         "\033[37m"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{//https://stackoverflow.com/questions/4954140/how-to-redirect-qdebug-qwarning-qcritical-etc-output
//    if (QThread::currentThread() != debuger->thread())
//        return;

    QByteArray localMsg = msg.toLocal8Bit();
    QString outputSring;
    switch (type) {
    case QtDebugMsg:
        outputSring = outputSring.asprintf(GREE_BEGIN "Debug: %s (%s:%u, %s)\n" END_COLOR, localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        outputSring = outputSring.asprintf(BLUE_BEGIN "Info: %s (%s:%u, %s)\n" END_COLOR, localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        outputSring = outputSring.asprintf(YELLOW_BEGIN "Warning: %s (%s:%u, %s)\n" END_COLOR, localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        outputSring = outputSring.asprintf(RED_BEGIN "Critical: %s (%s:%u, %s)\n" END_COLOR, localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        outputSring = outputSring.asprintf(RED_BEGIN "Fatal: %s (%s:%u, %s)\n" END_COLOR, localMsg.constData(), context.file, context.line, context.function);
//        abort();
    }

    if(debuger != nullptr)
        debuger->putMessadge(type, context, msg);

    if (QThread::currentThread() != debuger->thread())
        return;

    QTextStream(stdout) << outputSring;
}


int main(int argc, char *argv[])
{
#ifdef QCUSTOMPLOT_USE_OPENGL
    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
#endif
    QApplication a(argc, argv);
    qRegisterMetaType< QVector<uint8_t> >( "QVector<uint8_t>" );
    qRegisterMetaType< QVector<uint32_t> >( "QVector<uint32_t>" );
    qRegisterMetaType< QVector<uint32_t> >( "QVector<float>" );
    qRegisterMetaType< QVector<uint32_t> >( "QVector<double>" );
    qRegisterMetaType< uint32_t >( "uint32_t" );
    qRegisterMetaType< uint64_t >( "uint64_t" );
    qRegisterMetaType< varloc_location_t >( "varloc_location_t" );
    DebugerWindow _debuger;
    debuger = &_debuger;
    qInstallMessageHandler(myMessageOutput); // Install the handler
    STPlotWindow w(debuger);
    w.show();
    return a.exec();
}
