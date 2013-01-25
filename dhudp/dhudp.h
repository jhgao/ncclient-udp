#ifndef DHUDP_H
#define DHUDP_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>
#include "datahandler.h"
#include "execthread.h"
#include "protocol/protocoltypes.h"
#include "protocol/packet.h"

#include "dhudpprotocol.h"
#include "dhudpdecoder.h"
#include "dhudprcvqueue.h"

namespace nProtocUDP{

static const int  WAIT_FOR_CONNECTED_TIMEOUT = 5000;
static const int  WAIT_FOR_BYTES_WRITTEN_TIMEOUT = 5000;  //5s
static const int QUEUE_LIMIT_SIZE = 150;

class DHudp : public DataHandler
{
    Q_OBJECT
public:
    explicit DHudp(QObject *parent = 0);
    eProtocTypes type() const;
    QByteArray declareArg();
    
signals:
    void sig_cmdConnected();
    
public slots:
    void startFetch();
    void abortWorks();

private slots:
    void onIncomingTcpCmdConnection();
    void onCmdSktConnected();
    void onCmdSktReadyRead();
    void onCmdSktDisconnected();
    void onGotFullFile();

    //
    void readDatagram();
    void sendCmdNext();
    void sendCmdToCyc(quint32);
private:
    void writeOutCmd(quint16, const QByteArray& = QByteArray());
    void processCMD(const Packet& p);
    QString psCmdDbg(QString cmd, QString arg = QString());

    bool startListenData();
    void enqueueIncomingData(const QByteArray&);

    QTcpServer* i_tcpCmdServer;
    QString i_ipAddress;    //local ip
    QTcpSocket* i_tcpCmdSkt;
    QUdpSocket* i_udpDataSkt;
    int i_cmd_counter;

    quint16 i_cmdPacketSize;   //used when nonblocking rcv
    quint16 i_dataPacketSize;
    DHudpDecoder *i_decoder;
    ExecThread *i_decoderThread;

    //rcv datagram queue
    DHudpRcvQueue* i_queue;
    QTimer* i_procQueueDelayTimer;
};
}//namespace nProtocUDP

#endif // DHUDP_H
