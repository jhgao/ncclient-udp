#ifndef DHUDPDECODER_H
#define DHUDPDECODER_H

#include <QObject>
#include "decparams.h"

namespace nProtocUDP{
class DHudpDecoder : public QObject
{
    Q_OBJECT
public:
    explicit DHudpDecoder(QObject *parent = 0);
    void setDecodeParameters(const DecParams& p);
    
signals:
    
public slots:
private:
    DecParams i_params;
};
}//namespace nProtocUDP

#endif // DHUDPDECODER_H
