#ifndef DHUDPDECODER_H
#define DHUDPDECODER_H

#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QBitArray>
#include <QSet>
#include <QFileInfo>
#include "decparams.h"
#include "dhudprcvqueue.h"
#include "dhudpprotocol.h"

#include "rcvblock.h"

namespace nProtocUDP{

static const QString RCVER_CACHE_FILE = "dhudp.rcvcache";
static const int DECODE_QUEUE_DELAY_TIMEOUT = 150;
static const int WRONG_FRAGS_TOLERATION = ((ENC_BLOCK_SIZE)/(FRAGMENT_SIZE) +1);

class DHudpDecoder : public QObject
{
    Q_OBJECT
public:
    explicit DHudpDecoder(QObject *parent = 0);
    void setDecodeParameters(const DecParams&);
    void enqueueIncomingData(const QByteArray&);
    
signals:
    void sig_correctionFragCyc(quint32);
    void sig_needNextCycle();

    void sig_progressPercent(uint);
    void sig_savedBlockSN(quint32);
public slots:
    void processQueue();

private:
    void receiveFragment(const QByteArray&);
    bool checkCurrentCycleBlocks();
    bool saveCurrentCycleBlocks();
    void markGotBlock(const RcvBlock&);

    DecParams i_params;
    quint64 i_rcvAllBlocksNum;  //TODO: rename:total blocks number excepted to get
    DHudpRcvQueue* i_queue;
    QTimer* i_procQueueDelayTimer;

    //receiveFragment
    int i_wrongFragsCounter;
    quint32 i_rcv_cyc;
    QList<RcvBlock> i_rcvCycleBlocks;   //already got blocks
    quint64 i_currentCycleBlockNum;

    QBitArray   i_rcvBitMap;    //map of got / not got blocks
    QSet<quint32> i_gotBlockSNs;    //ID number of already got blocks

    //cache file
    QFileInfo   i_rcvCacheFileInfo;
};
}//namespace nProtocUDP

#endif // DHUDPDECODER_H
