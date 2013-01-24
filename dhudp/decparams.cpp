#include "decparams.h"

namespace nProtocUDP{
DecParams::DecParams():
    rawFileLength(0),
    inBlockCoeffLen(0),
    inBlockDataSize(0),
    totalEncBlocks(0),
    oneCycleBlockNum(0),
    totalCycleNum(0)
{
}

DecParams::DecParams(const QByteArray &a):
    rawFileLength(0),
    inBlockCoeffLen(0),
    inBlockDataSize(0),
    totalEncBlocks(0),
    oneCycleBlockNum(0),
    totalCycleNum(0)
{
    this->fromArray(a);
}

DecParams &DecParams::operator =(const DecParams &rhs)
{
    rawFileLength = rhs.rawFileLength;
    inBlockCoeffLen = rhs.inBlockCoeffLen;
    inBlockDataSize = rhs.inBlockDataSize;
    totalEncBlocks = rhs.totalEncBlocks;
    oneCycleBlockNum = rhs.oneCycleBlockNum;
    totalCycleNum = rhs.totalCycleNum;
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
    fs << oneCycleBlockNum;
    fs << totalCycleNum;
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
    in >> oneCycleBlockNum;
    in >>totalCycleNum;
    return true;
}

QString DecParams::dbgString() const
{
    return QString("DecParams-")
            + QString(" rawFile") + QString::number(rawFileLength)
            + QString(" coeff") + QString::number(inBlockCoeffLen)
            + QString(" blockData") + QString::number(inBlockDataSize)
            + QString(" totalBlock") + QString::number(totalEncBlocks)
            + QString(" oneCycleBlock") + QString::number(oneCycleBlockNum)
            + QString(" totalCycle") + QString::number(totalCycleNum);
}
}//namespace nProtocUDP
