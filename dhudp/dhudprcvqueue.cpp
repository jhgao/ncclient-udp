#include "dhudprcvqueue.h"
#include <QDebug>

namespace nProtocUDP{
DHudpRcvQueue::DHudpRcvQueue(const int triggerLimit, QObject *parent) :
    QObject(parent),i_triggerLimit(triggerLimit)
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
    if( 0 == (i_queue.size() % i_triggerLimit) )
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
