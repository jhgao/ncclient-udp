#include "rawblock.h"

RawBlock::RawBlock()
{
}

QByteArray RawBlock::toArray()
{
    QByteArray a;
    QDataStream out(&a, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << fileSize;
    out << offsetFrom;
    out << offsetTo;
    out << data;

    return a;
}

void RawBlock::fromArray(const QByteArray &a)
{
    QDataStream in(a);
    in.setVersion(QDataStream::Qt_4_8);
    in >> fileSize;
    in >> offsetFrom;
    in >> offsetTo;
    in >> data;
}
