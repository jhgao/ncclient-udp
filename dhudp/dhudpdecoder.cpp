#include "dhudpdecoder.h"

namespace nProtocUDP{

DHudpDecoder::DHudpDecoder(QObject *parent) :
    QObject(parent),i_queue(0),i_procQueueDelayTimer(0)
{
    i_queue = new DHudpRcvQueue(this);
    connect(i_queue, SIGNAL(sig_readyRead()),
            this, SLOT(processQueue()));

    i_procQueueDelayTimer = new QTimer(this);
    i_procQueueDelayTimer->setSingleShot(true);
    connect(i_procQueueDelayTimer,SIGNAL(timeout()),
            this, SLOT(processQueue()));
}

void DHudpDecoder::setDecodeParameters(const DecParams &p)
{
    qDebug() << "DHudpDecoder::setDecodeParameters()";
    i_params = p;
}

void DHudpDecoder::enqueueIncomingData(const QByteArray &a)
{
    i_queue->waitForEnqueue(a);
    i_procQueueDelayTimer->start(DECODE_QUEUE_DELAY_TIMEOUT);
}

void DHudpDecoder::processQueue()
{
    qDebug() << "DHudpDecoder::processQueue()";
    while(!i_queue->isEmpty()){
        qDebug() << "\t" << i_queue->size();
        receiveFragment(i_queue->waitForDequeue());
    }
}

void DHudpDecoder::receiveFragment(const QByteArray &a)
{
    qDebug() << "DHudpDecoder::receiveFragment()"
             << QString(a);
}

}
