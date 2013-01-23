#include "dhudpdecoder.h"

namespace nProtocUDP{

DHudpDecoder::DHudpDecoder(QObject *parent) :
    QObject(parent)
{
}

void DHudpDecoder::setDecodeParameters(const DecParams &p)
{
    qDebug() << "DHudpDecoder::setDecodeParameters()";
    i_params = p;
}
}
