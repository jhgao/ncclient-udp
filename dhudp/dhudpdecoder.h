#ifndef DHUDPDECODER_H
#define DHUDPDECODER_H

#include <QObject>


namespace nProtocUDP{
class DHudpDecoder : public QObject
{
    Q_OBJECT
public:
    explicit DHudpDecoder(QObject *parent = 0);
    
signals:
    
public slots:
    
};
}//namespace nProtocUDP

#endif // DHUDPDECODER_H
