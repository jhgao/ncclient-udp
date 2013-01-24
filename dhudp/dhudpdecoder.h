#ifndef DHUDPDECODER_H
#define DHUDPDECODER_H

#include <QObject>
#include <QQueue>
#include <QTimer>
#include "decparams.h"
#include "dhudprcvqueue.h"

namespace nProtocUDP{

static const int DECODE_QUEUE_DELAY_TIMEOUT = 150;

class DHudpDecoder : public QObject
{
    Q_OBJECT
public:
    explicit DHudpDecoder(QObject *parent = 0);
    void setDecodeParameters(const DecParams&);
    void enqueueIncomingData(const QByteArray&);
    
signals:
    
public slots:
    void processQueue();

private:
    void receiveFragment(const QByteArray&);
    DecParams i_params;
    DHudpRcvQueue* i_queue;
    QTimer* i_procQueueDelayTimer;
};
}//namespace nProtocUDP

#endif // DHUDPDECODER_H
