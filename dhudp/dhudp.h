#ifndef DHUDP_H
#define DHUDP_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include "datahandler.h"
#include "protocol/protocoltypes.h"
#include "protocol/packet.h"

#include "dhudpprotocol.h"
#include "dhudpdecoder.h"

namespace nProtocUDP{

static const int  WAIT_FOR_CONNECTED_TIMEOUT = 5000;
static const int  WAIT_FOR_BYTES_WRITTEN_TIMEOUT = 5000;  //5s

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
private slots:
    void onIncomingTcpCmdConnection();
    void onCmdSktReadyRead();
    void onCmdSktDisconnected();
private:
    void writeOutCmd(quint16, const QByteArray& = QByteArray());
    void processCMD(const Packet& p);
    QString psCmdDbg(QString cmd, QString arg = QString());
    void processData(const Packet& p);

    QTcpServer* i_tcpCmdServer;
    QString i_ipAddress;    //local ip
    QTcpSocket* i_tcpCmdSkt;
    QUdpSocket* i_udpDataSkt;
    int i_cmd_counter;

    quint16 i_cmdPacketSize;   //used when nonblocking rcv
    quint16 i_dataPacketSize;
    DHudpDecoder *i_decoder;
};
}//namespace nProtocUDP

#endif // DHUDP_H
