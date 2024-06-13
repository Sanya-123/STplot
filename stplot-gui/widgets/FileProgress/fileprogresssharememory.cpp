#include "fileprogresssharememory.h"
#include <QDebug>

FileProgressShareMemory::FileProgressShareMemory(QObject *parent)
    : QObject{parent}
{
    sharedMemoryProcent.setKey(MOMORY_KEY_PROCENT);
    sharedMemoryMsg.setKey(MOMORY_KEY_MSG);
    sharedMemoryCancelation.setKey(MOMORY_KEY_CANCEL);
    newProcent = 0;
    newMsg = "";
    newCancelation = false;
}

float FileProgressShareMemory::getProcent()
{
    newProcent = readShareMemoryType<float>(sharedMemoryProcent, newProcent);
    return newProcent;
}

QString FileProgressShareMemory::getMessadge()
{
    newMsg = readShareMemoryType<QString>(sharedMemoryMsg, newMsg);
    return newMsg;
}

bool FileProgressShareMemory::getCancelFlag()
{
    newCancelation = readShareMemoryType<bool>(sharedMemoryCancelation, newCancelation);
    return newCancelation;
}

void FileProgressShareMemory::setProcent(float procent)
{
    newProcent = procent;
    writeShareMemoryType(sharedMemoryProcent, procent);
}

void FileProgressShareMemory::setMessadge(QString msg)
{
    newMsg = msg;
    writeShareMemoryType(sharedMemoryMsg, msg);
}

void FileProgressShareMemory::setCancelFlag(bool cancel)
{
    newCancelation = cancel;
    writeShareMemoryType(sharedMemoryCancelation, cancel);
}

int FileProgressShareMemory::readShareMemory(QSharedMemory &mem, QBuffer &buffer)
{
    if (!mem.attach()) {
//        qDebug() << tr("Unable to attach to shared memory segment.\n" \
//                       "Load an image first:") << mem.key();
        return -1;
    }

    mem.lock();
    buffer.setData((char*)mem.constData(), mem.size());

    return 0;
}

void FileProgressShareMemory::endReadMemory(QSharedMemory &mem)
{
    mem.unlock();

    mem.detach();
}

int FileProgressShareMemory::writeShareMemory(QSharedMemory &mem, QBuffer &buffer)
{
    if (mem.isAttached())
        detach(mem);
    if (!mem.create(buffer.size())) {
        if (mem.error() == QSharedMemory::AlreadyExists)
        {
            mem.attach();
        } else {
            qDebug() << tr("Unable to create or attach to shared memory segment: %1")
                        .arg(mem.errorString());
            return -1;
        }
    }
    mem.lock();
    char *to = (char*)mem.data();
    const char *from = buffer.data().data();
    memcpy(to, from, qMin(mem.size(), (int)buffer.size()));
    mem.unlock();

    return 0;
}

/*!
  This private function is called by the destructor to detach the
  process from its shared memory segment. When the last process
  detaches from a shared memory segment, the system releases the
  shared memory.
 */
void FileProgressShareMemory::detach(QSharedMemory &mem)
{
    if (!mem.detach())
        qDebug() << "Unable to detach from shared memory:" << mem.key();
}
