#include "dhudprcvqueue.h"
#include <QDebug>

namespace nProtocUDP{
DHudpRcvQueue::DHudpRcvQueue(QObject *parent) :
    QObject(parent)
{
}

QByteArray DHudpRcvQueue::waitForDequeue()
{
    QMutexLocker locker(&i_mutex);
    return i_queue.dequeue();
}

void DHudpRcvQueue::waitForEnqueue(const QByteArray &a)
{
    QMutexLocker locker(&i_mutex);
    i_queue.enqueue(a);
    if( i_queue.size() > QUEUE_LIMIT_SIZE )
        emit sig_readyRead();
}

bool DHudpRcvQueue::isEmpty() const
{
    return i_queue.isEmpty();
}

int DHudpRcvQueue::size() const
{
    return i_queue.size();
}
}//namespace nProtocUDP
