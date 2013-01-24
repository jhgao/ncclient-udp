#ifndef DECPARAMS_H
#define DECPARAMS_H

#include <QByteArray>
#include <QIODevice>
#include <QDataStream>
#include <QDebug>

namespace nProtocUDP{

class DecParams
{
public:
    explicit DecParams();
    explicit DecParams(const QByteArray& a);
    DecParams& operator=(const DecParams&);

    QByteArray toArray() const;
    bool fromArray(const QByteArray &a);
    QString dbgString()const;

    //about raw file
    quint64 rawFileLength;

    //about encoding
    quint32 inBlockCoeffLen;    //coeffLen in incoming Blocks
    quint32 inBlockDataSize;
    quint64 totalEncBlocks;

    //about cycling
    quint32 oneCycleBlockNum;
    quint32 totalCycleNum;
};

}//namespace nProtocUDP
#endif // DECPARAMS_H
