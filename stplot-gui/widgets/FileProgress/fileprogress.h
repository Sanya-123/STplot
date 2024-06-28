#ifndef FILEPROGRESS_H
#define FILEPROGRESS_H

#include <QObject>
#include "readwritedevice.h"
#include "fileprogresssharememory.h"
#include <QProcess>
#include <QTimer>

#define PROGRESS_PROGRAMM_NAME          "FileProgressDialog"

/**
 * @brief The FileProgress class class that run progress dialog
 *          and communicati with it over share memoryt
 */
class FileProgress : public AbstractFileProgress
{
    Q_OBJECT
public:
    explicit FileProgress(QObject *parent = nullptr);
    //TODO cancelation flag
public slots:
    void startProgress();
    void stopProgress();
    void setProgress(float procent);
    void setMessadge(QString msg);

private slots:
    void checkCancelation();

private:
    QProcess fileProgressApplication;
    FileProgressShareMemory shareMemory;
    QTimer timerCheckCancelation;
};

#endif // FILEPROGRESS_H
