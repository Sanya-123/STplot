#ifndef FILEPROGRESSSHAREMEMORY_H
#define FILEPROGRESSSHAREMEMORY_H

#include <QObject>
#include <QSharedMemory>
#include <QBuffer>
#include <QDataStream>

#define MOMORY_KEY_PROCENT      "stplot/fileloader/procent"
#define MOMORY_KEY_MSG          "stplot/fileloader/msg"
#define MOMORY_KEY_CANCEL       "stplot/fileloader/cancel"

/**
 * @brief The FileProgressShareMemory class - common share memory
 *              for communicate beatven stplot-gui and FileProgressDialog
 */
class FileProgressShareMemory : public QObject
{
    Q_OBJECT
public:
    explicit FileProgressShareMemory(QObject *parent = nullptr);

    float getProcent();
    QString getMessadge();
    bool getCancelFlag();

public slots:
    void setProcent(float procent);
    void setMessadge(QString msg);
    void setCancelFlag(bool cancel);

private:
    int readShareMemory(QSharedMemory &mem, QBuffer &buffer);
    void endReadMemory(QSharedMemory &mem);
    int writeShareMemory(QSharedMemory &mem, QBuffer &buffer);
    void detach(QSharedMemory &mem);
    template<typename T>
    T readShareMemoryType(QSharedMemory &mem, T resIfNoteAcces)
    {
        T res = resIfNoteAcces;
        QBuffer buffer;
        QDataStream in(&buffer);
        if(readShareMemory(mem, buffer) == 0)
        {
            buffer.open(QBuffer::ReadOnly);
            in >> res;
        }
        endReadMemory(mem);
        return res;
    }
    template<typename T>
    int writeShareMemoryType(QSharedMemory &mem, T data)
    {
        QBuffer buffer;
        buffer.open(QBuffer::ReadWrite);
        QDataStream out(&buffer);
        out << data;

        return writeShareMemory(mem, buffer);
    }

signals:

private:
    QSharedMemory sharedMemoryProcent, sharedMemoryMsg, sharedMemoryCancelation;
    float newProcent;
    QString newMsg;
    bool newCancelation;


};

#endif // FILEPROGRESSSHAREMEMORY_H
