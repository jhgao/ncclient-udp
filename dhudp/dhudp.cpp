#include "dhudp.h"
namespace DHudp{
DHudp::DHudp(QObject *parent) :
    DataHandler(parent)
{
}

eProtocTypes DHudp::type() const
{
    return PROTOC_UDP;
}

QByteArray DHudp::declareArg()
{
    return QByteArray();
}

void DHudp::startFetch()
{
}

void DHudp::abortWorks()
{
}
}
