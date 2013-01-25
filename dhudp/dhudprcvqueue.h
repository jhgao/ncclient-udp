#ifndef DHUDPRCVQUEUE_H
#define DHUDPRCVQUEUE_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>

namespace nProtocUDP{

class DHudpRcvQueue : public QObject
{
    Q_OBJECT
public:
    explicit DHudpRcvQueue(const int triggerLimit = 500,
                           QObject *parent = 0);
    QByteArray waitForDequeue();
    void waitForEnqueue(const QByteArray&);
    void waitForClear();
    bool isEmpty()const;
    int	size () const;
signals:
    void sig_readyRead();
public slots:
private:
    QQueue<QByteArray> i_queue;
    QMutex* i_mutex;
    const int i_triggerLimit;
};
}//namespace nProtocUDP

#endif // DHUDPRCVQUEUE_H
