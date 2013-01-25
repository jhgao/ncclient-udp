#ifndef DHUDPDECODER_H
#define DHUDPDECODER_H

#include <QObject>
#include <QQueue>
#include <QBitArray>
#include <QSet>
#include <QFileInfo>
#include "decparams.h"
#include "dhudprcvqueue.h"
#include "dhudpprotocol.h"

#include "rcvblock.h"

namespace nProtocUDP{

static const QString RCVER_CACHE_FILE = "dhudp.rcvcache";
static const int PROCESS_QUEUE_DELAY_TIMEOUT = 150;
static const int WRONG_FRAGS_TOLERATION = ((ENC_BLOCK_SIZE)/(FRAGMENT_SIZE) +1);

class DHudpDecoder : public QObject
{
    Q_OBJECT
public:
    explicit DHudpDecoder(DHudpRcvQueue &, QObject *parent = 0);
    void setDecodeParameters(const DecParams&);
    
signals:
    void sig_correctionCyc(quint32);
    void sig_needNextCycle();
    void sig_fullFileSaved();

    void sig_progressPercent(uint);
    void sig_gotBlockSN(quint32);
public slots:
    void processQueue();

private:
    bool isParamsReady();
    void initRcvBitMapFromBlocksNum(quint64 bn = 0);
    void clearRcvBlocksCacheForCycle(quint32 cyc);
    bool processFragment(const QByteArray&);
    void correctCycleTo(quint32);    //can be called repeatly
    bool checkCurrentCycleBlocks();
    void onGotAllCurrentCycleBlocks();
    bool saveCurrentCycleBlocks();
    void toCycle(quint32);
    void markSavedBlock(const RcvBlock&);

    quint32 blockNumInCycle(quint32) const;
    bool touch(QString aFilePath);

    DecParams i_params;
    quint64 i_rcvAllBlocksNum;  //TODO: rename:total blocks number excepted to get
    DHudpRcvQueue& i_queue;

    //receiveFragment
    int i_wrongFragsCounter;
    quint32 i_rcv_cyc;
    QList<RcvBlock> i_rcvCycleBlocks;   //already got blocks

    QBitArray   i_rcvBitMap;    //map of got / not got blocks
    QSet<quint32> i_gotBlockSNs;    //ID number of already got blocks

    //cache file
    QFileInfo   i_rcvCacheFileInfo;
};
}//namespace nProtocUDP

#endif // DHUDPDECODER_H
