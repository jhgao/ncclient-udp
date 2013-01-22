#ifndef DHUDP_H
#define DHUDP_H

#include "datahandler.h"
#include "protocol/protocoltypes.h"

namespace DHudp{
class DHudp : public DataHandler
{
    Q_OBJECT
public:
    explicit DHudp(QObject *parent = 0);
    eProtocTypes type() const;
    QByteArray declareArg();
    
signals:
    
public slots:
    void startFetch();
    void abortWorks();
    
};
}

#endif // DHUDP_H