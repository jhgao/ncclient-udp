#include "rcvblock.h"
#include <QDebug>

namespace nProtocUDP{
RcvBlock::RcvBlock(quint16 acyc, quint8 ablockNo, quint16 tgtSize):
    cyc(acyc),blockNo(ablockNo),tgtSize(tgtSize),gotSize(0)
{
    gotFrags.clear();
}

quint16 RcvBlock::assembleFragment(Fragment &f){
    //filter
    if( f.cyc != cyc) return 0;
    if( gotFrags.contains(f.fragNo)) return 0;

    //adjust target size
    if( 0 < f.blockSize && f.blockSize < 2*ENC_BLOCK_SIZE)
        tgtSize = f.blockSize;

    //save
    data.replace((int)f.offset,
                        (int)f.dataSize(),
                        f.data);
    //update records
    gotSize += f.dataSize();
    gotFrags.insert(f.fragNo);

    //debug
//    qDebug() << this->dbgString()
//             << "frag" << f.fragNo << "/got" << gotFrags.size();
    if (gotSize > tgtSize) {
        qDebug() << gotSize << "/" << tgtSize
                 << "@ frag" << f.cyc << "." << f.fragNo;
    }
    return f.dataSize();
}

bool RcvBlock::isComplete()
{
    /*
     * aRcvBlock is not a Encoded Block Data,
     * RcvBlock contains header bytes, its bigger than a Encoded Block Data.
     */
    return gotSize >= tgtSize;
}

QString RcvBlock::dbgString() const
{
    return QString("(g")
            + QString::number(this->gotSize)
            + QString("/d")
            + QString::number(this->data.size())
            + QString("/t")
            + QString::number(this->tgtSize)
            + QString(" bytes)")
            + QString::number(this->cyc) + "."
            + QString::number(this->blockNo);
}
}//namespace nProtocUDP

