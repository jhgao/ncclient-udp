#include "decparams.h"

namespace nProtocUDP{
DecParams::DecParams()
{
}

DecParams::DecParams(const QByteArray &a)
{
    this->fromArray(a);
}

DecParams &DecParams::operator =(const DecParams &rhs)
{
    rawFileLength = rhs.rawFileLength;
    inBlockCoeffLen = rhs.inBlockCoeffLen;
    inBlockDataSize = rhs.inBlockDataSize;
    totalEncBlocks = rhs.totalEncBlocks;
    return *this;
}

QByteArray DecParams::toArray() const
{
    QByteArray f;
    QDataStream fs(&f,QIODevice::WriteOnly);
    fs.setVersion(QDataStream::Qt_4_8);
    fs << rawFileLength;
    fs << inBlockCoeffLen;
    fs << inBlockDataSize;
    fs << totalEncBlocks;
    return f;
}

bool DecParams::fromArray(const QByteArray &a)
{
    QDataStream in(a);
    in.setVersion(QDataStream::Qt_4_8);
    in >> rawFileLength;
    in >> inBlockCoeffLen;
    in >> inBlockDataSize;
    in >> totalEncBlocks;
    return true;
}

QString DecParams::dbgString() const
{
    return QString("DecParams-")
            + QString(" rawFile") + QString::number(rawFileLength)
            + QString(" coeff") + QString::number(inBlockCoeffLen)
            + QString(" blockData") + QString::number(inBlockDataSize)
            + QString(" totalBlock") + QString::number(totalEncBlocks);
}
}//namespace nProtocUDP
