#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QTime>
#include "protocol/packet.h"
#include "protocol/protocoltypes.h"
#include "datahandler.h"

//datahandler & encoders
#include "dhudp/dhudp.h"

class Connection : public QTcpSocket
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = 0);
    
signals:
    void sig_ConnectionFinished(Connection*);
    void sig_progressPercent(uint);
    void sig_gotBlockSN(quint32 sn);

public slots:
    void slot_abort();
    void slot_connectToHost(QString, quint16);

private slots:
    void onControlSktReadyRead();
    void processCMD(const Packet& p);

    void writeOutCMD(eControl_CMD cmd,
                     const QByteArray arg = QByteArray());
    void onConnected();

private:
    QString psCmdDbg(QString cmd, QString arg = QString());
    void processProtocolAck(eProtocTypes type, const QByteArray protocArg);
    quint16 packetSize; //for reading packet
    int i_cmd_counter;

    //protocol ack
    quint16     i_ackProtoc;
    QByteArray     i_ackProtocArg;

    //dh
    DataHandler *i_dh;
};

#endif // CONNECTION_H
