#ifndef DHUDPRCVQUEUE_H
#define DHUDPRCVQUEUE_H

#include <QObject>
#include <QQueue>

#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>

namespace nProtocUDP{

static const int QUEUE_LIMIT_SIZE = 500;

class DHudpRcvQueue : public QObject
{
    Q_OBJECT
public:
    explicit DHudpRcvQueue(QObject *parent = 0);
    QByteArray waitForDequeue();
    void waitForEnqueue(const QByteArray&);
    bool isEmpty()const;
    int	size () const;
signals:
    void sig_readyRead();
public slots:
private:
    QQueue<QByteArray> i_queue;
    QReadWriteLock i_mutex;
};
}//namespace nProtocUDP

#endif // DHUDPRCVQUEUE_H
