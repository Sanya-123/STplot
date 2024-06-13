#include "fileprogress.h"

FileProgress::FileProgress(QObject *parent)
    : AbstractFileProgress{parent}
{
    fileProgressApplication.setProgram(PROGRESS_PROGRAMM_NAME);
#if defined(Q_OS_WIN)
    fileProgressApplication.setProgram(QString(PROGRESS_PROGRAMM_NAME) + ".exe");
#endif
//    fileProgressApplication.start();
    shareMemory.setCancelFlag(true);
    shareMemory.setMessadge("");
    shareMemory.setProcent(0);
    timerCheckCancelation.setInterval(100);
    connect(&timerCheckCancelation, SIGNAL(timeout()), this, SLOT(checkCancelation()));
}

void FileProgress::startProgress()
{
    AbstractFileProgress::startProgress();
    shareMemory.setCancelFlag(false);
    timerCheckCancelation.start();
    fileProgressApplication.start();
}

void FileProgress::stopProgress()
{
    AbstractFileProgress::stopProgress();
    shareMemory.setCancelFlag(true);
    timerCheckCancelation.stop();
    fileProgressApplication.close();
}

void FileProgress::setProgress(float procent)
{
    shareMemory.setProcent(procent);
}

void FileProgress::setMessadge(QString msg)
{
    shareMemory.setMessadge(msg);
}

void FileProgress::checkCancelation()
{
    canceFlag = shareMemory.getCancelFlag();
//    if(canceFlag)
//        qDebug() << "stop runing flug";
}
