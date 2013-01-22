#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>
#include "protocol/cmd_define.h"
#include "protocol/protocoltypes.h"

class DataHandler : public QObject
{
    Q_OBJECT
public:
    explicit DataHandler(QObject *parent = 0);

    virtual eProtocTypes type() const = 0;
    /* used when connect to Server */
    virtual QByteArray declareArg() = 0;
signals:
    void sig_writeOutCmd(eControl_CMD,QByteArray);

    void sig_progressPercent(uint);
    void sig_gotBlockSN(quint32 sn);
    
public slots:
    /* called when server is ready with chooesed protocol */
    virtual void startFetch() = 0;
    virtual void abortWorks() = 0;
protected:
    /* signal parent object to write out CMD */
    void sigWriteOutCmd(eControl_CMD, const QByteArray);
};

#endif // DATAHANDLER_H
