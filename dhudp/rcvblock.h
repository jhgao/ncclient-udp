#ifndef RCVBLOCK_H
#define RCVBLOCK_H

#include <QSet>
#include "fragment.h"
#include "consts.h"

namespace nProtocUDP{
class RcvBlock
{
public:
    RcvBlock(quint16 acyc, quint8 ablockNo, quint16 tgtSize);

    quint16 assembleFragment(Fragment &f);
    bool isComplete();
    QString dbgString() const;

    quint16 tgtSize;
    quint16 gotSize;
    quint16 cyc;    //cycle number
    quint8 blockNo; //in cycle block number
    QByteArray data;
    QSet<quint16> gotFrags; // received fragmets' number set
};
}//namespace nProtocUDP
#endif // RCVBLOCK_H
