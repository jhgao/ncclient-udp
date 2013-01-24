/*
 * Fragment: transfer unit using UDP
 */
#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <QByteArray>
#include <QIODevice>
#include <QDataStream>
#include <QDebug>

namespace nProtocUDP{
class Fragment
{
public:
    Fragment();
    Fragment(quint16 acyc, quint8 ablockNo, quint16 ablockSize,
             quint16 afragNo, quint16 aoffset, QByteArray adata);

    QByteArray toArray();
    quint16 fromArray(QByteArray a);
    int dataSize();
    QString dbgString() const;

    //Fragment segments
    //quint16       //Fragment total size
    quint16 cyc;    //cycle number
    quint8 blockNo; //in cycle block number
    quint16 blockSize;

    quint16 fragNo; //in block fragment number
    quint16 offset; //in block offset
    QByteArray data;
    //end of Fragment
};
}//namespace nProtocUDP

#endif // FRAGMENT_H
