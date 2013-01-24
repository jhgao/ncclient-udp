#include "dhudpdecoder.h"

namespace nProtocUDP{

DHudpDecoder::DHudpDecoder(DHudpRcvQueue &q, QObject *parent) :
    QObject(parent),i_queue(q),
    i_wrongFragsCounter(0),i_rcv_cyc(0),i_rcvAllBlocksNum(0),
    i_currentCycleBlockNum(0)
{
    //cache file
    i_rcvCacheFileInfo.setFile(RCVER_CACHE_FILE);
    if(i_rcvCacheFileInfo.exists()){
        QFile::remove(i_rcvCacheFileInfo.filePath());
    }
    if(!touch(i_rcvCacheFileInfo.filePath()))
        qDebug() << "\t Error: failed to touch receive cache file";

    this->touch(i_rcvCacheFileInfo.fileName() + ".raw");
}

void DHudpDecoder::setDecodeParameters(const DecParams &p)
{
    qDebug() << "DHudpDecoder::setDecodeParameters()";
    i_params = p;
    i_rcvAllBlocksNum = i_params.totalEncBlocks;
    initRcvBitMapFromBlocksNum(i_rcvAllBlocksNum);
    //prepare for cycle 0
    this->clearRcvBlocksCacheForCycle(0);
}

void DHudpDecoder::processQueue()
{
//    qDebug() << "DHudpDecoder::processQueue()";
    while(!i_queue.isEmpty()){
        receiveFragment(i_queue.waitForDequeue());
    }
}

void DHudpDecoder::initRcvBitMapFromBlocksNum(quint64 bn)
{
    i_rcvBitMap.resize(bn);
    i_rcvBitMap.fill(false);
    i_gotBlockSNs.clear();
}

void DHudpDecoder::clearRcvBlocksCacheForCycle(quint32 cyc)
{
    qDebug() << "DHudpDecoder::clearRcvBlocksCacheForCycle()" << cyc;
    i_rcvCycleBlocks.clear();
    for(int i = 0; i< i_currentCycleBlockNum; ++i){
        i_rcvCycleBlocks.append(RcvBlock(cyc,i));
    }
}

void DHudpDecoder::receiveFragment(const QByteArray &a)
{
    Fragment frag;
    if( 0 == frag.fromArray(a))
        return;

    qDebug() << "DHudpDecoder::receiveFragment()" << frag.dbgString();

    //filter fragmets
    if( frag.cyc != i_rcv_cyc ){
        ++i_wrongFragsCounter;
        if( i_wrongFragsCounter > WRONG_FRAGS_TOLERATION){
            i_wrongFragsCounter = 0;
            emit sig_correctionFragCyc(i_rcv_cyc);
        }
        return;
    }

    //assemble fragment into block
    if(i_rcvCycleBlocks.isEmpty()){
        qDebug() << "\t Err:rcv blocks cache list empty!";
        return;
    }

    RcvBlock &b = i_rcvCycleBlocks[frag.blockNo];
    b.assembleFragment(frag);

    if( b.isComplete() ){ //got this whole block
        QString dbg = QString("[got Block] ")
                + b.dbgString();
        qDebug() << dbg;
        this->markGotBlock(b);

        if(this->checkCurrentCycleBlocks()){
            this->saveCurrentCycleBlocks();
            this->toCycle(i_rcv_cyc + 1);
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

void DHudpDecoder::toCycle(quint32 cyc)
{
    if( cyc >= i_params.totalCycleNum ) return;

    i_rcv_cyc = cyc ;
    i_currentCycleBlockNum = blockNumInCycle(i_rcv_cyc);
    this->clearRcvBlocksCacheForCycle(i_rcv_cyc);
}

void DHudpDecoder::markGotBlock(const RcvBlock &b)
{
    quint32 blockSN = ONE_CYCLE_BLOCKS * b.cyc + b.blockNo ;
    i_rcvBitMap.setBit(blockSN, true);
    i_gotBlockSNs.insert(blockSN);

    emit sig_progressPercent(i_gotBlockSNs.size()*100/i_rcvAllBlocksNum);
}

quint32 DHudpDecoder::blockNumInCycle(quint32 cyc) const
{
    quint32 n;
    if( cyc + 1 < i_params.totalCycleNum
            || 0 == i_rcvAllBlocksNum % i_params.oneCycleBlockNum){
         n = ONE_CYCLE_BLOCKS;
    }else if( cyc +1 == i_params.totalCycleNum){
        n = i_rcvAllBlocksNum % i_params.oneCycleBlockNum;
    }else {
        n = 0;
    }
    return n;
}

bool DHudpDecoder::touch(QString aFilePath)
{
    if( QFile::exists(aFilePath)) return true;

    QFile f(aFilePath);
    bool rst = f.open(QIODevice::ReadWrite);
    f.close();
    return rst;
}

}
