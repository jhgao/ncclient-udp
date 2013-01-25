#include "dhudpdecoder.h"

namespace nProtocUDP{

DHudpDecoder::DHudpDecoder(DHudpRcvQueue &q, QObject *parent) :
    QObject(parent),i_queue(q),
    i_wrongFragsCounter(0),i_rcv_cyc(0),i_rcvAllBlocksNum(0)
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
    if( !isParamsReady() ){
        qDebug() << "DHudpDecoder::processQueue()"
                 << "Err: parameter not ready";
        return;
    }

    while(!i_queue.isEmpty()){
        processFragment(i_queue.waitForDequeue());
    }
}

bool DHudpDecoder::isParamsReady()
{
    return ( 0 != i_params.oneCycleBlockNum )
            && ( 0 != i_params.totalEncBlocks )
            && ( 0 != i_params.totalCycleNum );
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

    quint32 blockNum  = blockNumInCycle(cyc);
    if( 0 == blockNum ){
        qDebug() << "\t Err: block number in this cycle is 0 !";
        return;
    }

    for(int i = 0; i< blockNum; ++i){
        i_rcvCycleBlocks.append(RcvBlock(cyc,i));   //TODO RcvBlock tgtSize
    }
}

bool DHudpDecoder::processFragment(const QByteArray &a)
{
    Fragment frag;
    if( 0 == frag.fromArray(a))
        return false;

//    qDebug() << "DHudpDecoder::processFragment()" << frag.dbgString();

    //filter fragmets
    if( frag.cyc != i_rcv_cyc ){
        qDebug() << "DHudpDecoder::processFragment()"
                 << "wrong frag " << frag.dbgString();
        ++i_wrongFragsCounter;
        this->correctCycleTo(i_rcv_cyc);
        return false;
    }else{
        i_wrongFragsCounter = 0;
    }

    //assemble fragment into block
    if(i_rcvCycleBlocks.isEmpty()){
        qDebug() << "\t Err: rcv blocks cache list empty!";
        return false;
    }

    RcvBlock &b = i_rcvCycleBlocks[frag.blockNo];
    if( ! b.isComplete() ){
        b.assembleFragment(frag);
        if( b.isComplete() ){ //got this whole block
            qDebug() << "[cached Block] " << b.dbgString();
            emit sig_gotBlockSN(i_params.oneCycleBlockNum*b.cyc + b.blockNo);

            //check if got all current cycle blocks
            if( this->checkCurrentCycleBlocks())
                this->onGotAllCurrentCycleBlocks();
        }
    }

    return true;
}

/* Important: can be called repeatly, without side effect */
void DHudpDecoder::correctCycleTo(quint32 cyc)
{
    if( i_wrongFragsCounter > WRONG_FRAGS_TOLERATION){
        i_wrongFragsCounter = 0;
        emit sig_correctionCyc(i_rcv_cyc);
    }
}

bool DHudpDecoder::checkCurrentCycleBlocks()
{
    qDebug() << "DataHandler::checkCurrentCycleBlocks()"
             << "cycle" << i_rcv_cyc;

    quint64 tgtNum = blockNumInCycle(i_rcv_cyc);
    qDebug() << "\t tgt" << tgtNum << "blocks";
    if( i_rcvCycleBlocks.size() != tgtNum)
        return false;

    for(int i = 0; i < i_rcvCycleBlocks.size(); ++i){
        if( !i_rcvCycleBlocks[i].isComplete() ){
            qDebug() << "\t block" << i << " not complete"
                        << i_rcvCycleBlocks[i].dbgString();
            return false;
        }
    }

    qDebug() << "\t Got all : current cycle blocks.";
    return true;
}

void DHudpDecoder::onGotAllCurrentCycleBlocks()
{
    this->saveCurrentCycleBlocks();

    //check if all saved
    for(int i = 0 ; i< i_rcvBitMap.size() ; ++i){
        if(!i_rcvBitMap.testBit(i)){    //if a block is absence
            //found its cycle no.
            quint32 tgtCycle = (i+1) / i_params.oneCycleBlockNum;
            //cmd server to send that cycle
            if(tgtCycle > i_rcv_cyc){
                this->toCycle(tgtCycle);
                emit sig_needNextCycle();
            }else{
                this->toCycle(tgtCycle);
                this->correctCycleTo(tgtCycle);
            }
            return;
        }
    }

    //all file is saved
    emit sig_fullFileSaved();

    i_queue.waitForClear();
}

bool DHudpDecoder::saveCurrentCycleBlocks()
{
    qDebug() << "DataHandler::saveCurrentCycleBlocks()"
             << "cyc" << i_rcv_cyc
             << "to" << i_rcvCacheFileInfo.fileName();

    QFile f(i_rcvCacheFileInfo.filePath());

    if( !f.open(QIODevice::WriteOnly | QIODevice::Append)){
        qDebug() << "\t error open rcv cache file";
        f.close();
        return false;
    }

    quint64 dataOffset = 0;
    quint64 wroteBytes = 0;
    int wResult = 0;

    for(int i = 0; i< i_rcvCycleBlocks.size(); ++i){
        const RcvBlock &b = i_rcvCycleBlocks.at(i);
        dataOffset = b.tgtSize * (i_params.oneCycleBlockNum * b.cyc + b.blockNo);
        f.seek(dataOffset);
        wResult = f.write(b.data.data(), b.tgtSize);
        if( -1 == wResult ){
            qDebug() << "\t write error" << f.errorString();
            break;
        }else {
            wroteBytes += wResult;
            this->markSavedBlock(b);
            qDebug() << "\t saved block" << b.dbgString();
            qDebug() << "\t wrote out" << wroteBytes << "bytes";
        }
    }

    f.close();
    return true;
}

void DHudpDecoder::toCycle(quint32 cyc)
{
    if( cyc >= i_params.totalCycleNum ) return;

    i_rcv_cyc = cyc ;
    this->clearRcvBlocksCacheForCycle(i_rcv_cyc);
}

void DHudpDecoder::markSavedBlock(const RcvBlock &b)
{
    quint32 blockSN = i_params.oneCycleBlockNum * b.cyc + b.blockNo ;
    i_rcvBitMap.setBit(blockSN, true);
    i_gotBlockSNs.insert(blockSN);

    emit sig_progressPercent(i_gotBlockSNs.size()*100/i_rcvAllBlocksNum);
}

quint32 DHudpDecoder::blockNumInCycle(quint32 cyc) const
{
    quint32 n;
    if( cyc + 1 < i_params.totalCycleNum
            || 0 == i_rcvAllBlocksNum % i_params.oneCycleBlockNum){
         n = i_params.oneCycleBlockNum;
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
