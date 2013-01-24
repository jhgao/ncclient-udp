#include "dhudpdecoder.h"

namespace nProtocUDP{

DHudpDecoder::DHudpDecoder(QObject *parent) :
    QObject(parent),i_queue(0),i_procQueueDelayTimer(0),
    i_wrongFragsCounter(0),i_rcv_cyc(0),i_rcvAllBlocksNum(0),
    i_currentCycleBlockNum(0)
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

    Fragment frag;
    if( 0 == frag.fromArray(a))
        return;

//    qDebug() << "receiveFragment()" << frag.dbgString();

    //filter fragmets
    if( frag.cyc != i_rcv_cyc ){
        ++i_wrongFragsCounter;
        if( i_wrongFragsCounter > WRONG_FRAGS_TOLERATION){
            i_wrongFragsCounter = 0;
            emit sig_correctionFragCyc(i_rcv_cyc);  //need current cycle fragments
        }
        return;
    }

    //assemble fragment into block
    RcvBlock &b = i_rcvCycleBlocks[frag.blockNo];
    b.assembleFragment(frag);

    if( b.isComplete() ){ //got this whole block
        QString dbg = QString("[got Block] ")
                + b.dbgString();
        qDebug() << dbg;
        this->markGotBlock(b);

        if(this->checkCurrentCycleBlocks()){
            this->saveCurrentCycleBlocks();
            emit sig_needNextCycle();
        }
    }
}

bool DHudpDecoder::checkCurrentCycleBlocks()
{
    qDebug() << "DataHandler::checkCurrentCycleBlocks()";
    quint64 limit;
    if( 0 < i_currentCycleBlockNum) limit = i_currentCycleBlockNum;
    else limit = ONE_CYCLE_BLOCKS;

    qDebug() << "\t tgt" << limit << "blocks";

    for(int i = 0; i< limit; ++i){
        if( !i_rcvCycleBlocks[i].isComplete() ){
            qDebug() << "\t block" << i << " not complete"
                        << i_rcvCycleBlocks[i].dbgString();
            return false;
        }
    }

    qDebug() << "\t Got all : current cycle blocks.";
    return true;
}

bool DHudpDecoder::saveCurrentCycleBlocks()
{
    qDebug() << "DataHandler::saveCurrentCycleBlocks()"
             << "to" << i_rcvCacheFileInfo.fileName();
    quint64 limit;
    if( 0 < i_currentCycleBlockNum) limit = i_currentCycleBlockNum;
    else limit = ONE_CYCLE_BLOCKS;

    if( 0 == limit) return false;

    QFile f(i_rcvCacheFileInfo.filePath());

    if( !f.open(QIODevice::WriteOnly | QIODevice::Append)){
        qDebug() << "\t error open rcv cache file";
        f.close();
        return false;
    }

    quint64 dataOffset = 0;
    quint64 wroteBytes = 0;
    int wResult = 0;

    for(int i = 0; i< limit; ++i){
        const RcvBlock &b = i_rcvCycleBlocks.at(i);
        dataOffset = b.tgtSize * (ONE_CYCLE_BLOCKS * b.cyc + b.blockNo);
        f.seek(dataOffset);
        wResult = f.write(b.data.data(), b.tgtSize);
        if( -1 == wResult ){
            qDebug() << "\t write error" << f.errorString();
            break;
        }else {
            wroteBytes += wResult;
            emit sig_savedBlockSN(ONE_CYCLE_BLOCKS*b.cyc + b.blockNo);
        }
    }

    f.close();

    qDebug() << "\t wrote out" << wroteBytes << "bytes";

    return true;
}

void DHudpDecoder::markGotBlock(const RcvBlock &b)
{
    quint32 blockSN = ONE_CYCLE_BLOCKS * b.cyc + b.blockNo ;
    i_rcvBitMap.setBit(blockSN, true);
    i_gotBlockSNs.insert(blockSN);

    emit sig_progressPercent(i_gotBlockSNs.size()*100/i_rcvAllBlocksNum);
}

}
