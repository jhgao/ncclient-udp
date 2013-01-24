#include "fragment.h"

namespace nProtocUDP{
Fragment::Fragment():
    cyc(0),blockNo(0),blockSize(0),fragNo(0),offset(0)
{
}

Fragment::Fragment(quint16 acyc, quint8 ablockNo, quint16 ablockSize, quint16 afragNo, quint16 aoffset, QByteArray adata):
    cyc(acyc),blockNo(ablockNo), blockSize(ablockSize),fragNo(afragNo),offset(aoffset), data(adata)
{
}

QByteArray Fragment::toArray(){
    QByteArray f;

    f.clear();
    QDataStream fs(&f,QIODevice::WriteOnly);
    fs.setVersion(QDataStream::Qt_4_0);
    fs << (quint16)0;
    fs << cyc;
    fs << blockNo;
    fs << blockSize;
    fs << fragNo;
    fs << offset;
    fs << data;
    fs.device()->seek(0);
    fs << (quint16)(f.size() - sizeof(quint16));
//        qDebug() << "\t Fragment::toArray()" <<  (quint16)(f.size() - sizeof(quint16));
    return f;
}

quint16 Fragment::fromArray(QByteArray a){
//        qDebug() << "Fragment::fromQByteArray()";
    if( a.size() < sizeof(quint16) ){
        qDebug() << "\t E:array size " << a.size();
        return 0;
    }

    data.clear();
    quint16 size;

    QDataStream in(&a, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_0);
    in >> size;
    if( a.size() - sizeof(quint16) < size || size == 0){
        qDebug() << "\t corrupt Fragment"
                 << a.size() - sizeof(quint16)
                 << "/" << size;
        return 0; //corrupted, abandon
    }
    in >> cyc;
    in >> blockNo;
    in >> blockSize;
    in >> fragNo;
    in >> offset;
    in >> data;
    return data.size();
}

int Fragment::dataSize()
{
    return this->data.size();
}

QString Fragment::dbgString() const{
    return QString("Frag-") + QString::number(cyc) + "."
            + QString::number(blockNo) + "."
            + QString::number(fragNo) + "."
            + "@" + QString::number(offset);
}
}//namespace nProtocUDP
