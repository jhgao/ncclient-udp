#include "dhudp.h"

namespace nProtocUDP{

DHudp::DHudp(QObject *parent) :
    DataHandler(parent),i_tcpCmdServer(0),i_tcpCmdSkt(0),
    i_cmd_counter(0),i_cmdPacketSize(0),i_dataPacketSize(0),
    i_decoder(0),i_udpDataSkt(0),i_decoderThread(0),i_queue(0),
    i_procQueueDelayTimer(0)
{
    qDebug() << "DHudp::DHudp()";

    i_udpDataSkt = new QUdpSocket(this);    

    i_queue = new DHudpRcvQueue(this);
    i_procQueueDelayTimer = new QTimer(this);
    i_procQueueDelayTimer->setSingleShot(true);
    i_decoder = new DHudpDecoder(*i_queue);

    connect(i_procQueueDelayTimer,SIGNAL(timeout()),//cross thread
            i_decoder, SLOT(processQueue()));
    connect(i_queue, SIGNAL(sig_readyRead()),//cross thread
            i_decoder, SLOT(processQueue()));

    connect(i_decoder, SIGNAL(sig_correctionFragCyc(quint32)),
            this, SLOT(sendCmdToCyc(quint32)));
    connect(i_decoder, SIGNAL(sig_needNextCycle()),
            this, SLOT(sendCmdNext()));
    connect(i_decoder, SIGNAL(sig_progressPercent(uint)),
            this, SIGNAL(sig_progressPercent(uint)));
    connect(i_decoder, SIGNAL(sig_savedBlockSN(quint32)),
            this, SIGNAL(sig_gotBlockSN(quint32)));

    i_decoderThread = new ExecThread(this);
    i_decoder->moveToThread(i_decoderThread);
    i_decoderThread->start();


    i_tcpCmdServer = new QTcpServer(this);
    if (!i_tcpCmdServer->listen(QHostAddress::Any,0)) {
        qDebug() << "DHudp listen cmd port failed";
        exit(-1);
    }

    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            i_ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (i_ipAddress.isEmpty())
        i_ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    qDebug() << "\n DHudp is listening cmd at" << i_ipAddress
             << ":" << i_tcpCmdServer->serverPort();

    connect(i_tcpCmdServer, SIGNAL(newConnection()),
            this, SLOT(onIncomingTcpCmdConnection()));

    //logic
    connect(this, SIGNAL(sig_cmdConnected()),
            this, SLOT(onCmdSktConnected()));
}

eProtocTypes DHudp::type() const
{
    return PROTOC_UDP;
}

QByteArray DHudp::declareArg()
{
    QByteArray arg;
    QDataStream out(&arg, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << i_ipAddress;
    out << (quint16) i_tcpCmdServer->serverPort();
    return arg;
}

void DHudp::startFetch()
{
    if(this->startListenData()){
        qDebug() << "DHudp::startFetch()";
        this->writeOutCmd(QUE_DECODE_PARAM);
    }
}

void DHudp::abortWorks()
{
}

void DHudp::onCmdSktConnected()
{
    qDebug() << "DHudp::onCmdSktConnected()";
    startFetch();
}

void DHudp::onCmdSktReadyRead()
{
    //get packet size
    QDataStream in(i_tcpCmdSkt);
    in.setVersion(QDataStream::Qt_4_8);
    if (i_cmdPacketSize == 0) {
        if (i_tcpCmdSkt->bytesAvailable() < (int)sizeof(quint16)){
            return;
        }
        in >> i_cmdPacketSize;
    }

    //ensure data size available
    if (i_tcpCmdSkt->bytesAvailable() < i_cmdPacketSize){
        return;
    }

    //read in data
    QByteArray payloadArrey;
    in >> payloadArrey;

    //analyze payload
    Packet p;
    if( p.fromPayload(payloadArrey)){
        switch(p.getType()){
        case PTYPE_CMD:
            processCMD(p);
            break;
        case PTYPE_DATA:
            qDebug() << "TODO: data on cmd skt";
            break;
        default:
            qDebug() << "\t unknown packet type";
        }
    }

    i_cmdPacketSize = 0;
}

void DHudp::onCmdSktDisconnected()
{
    qDebug() << "DHudp::onCmdSktDisconnected()";
}

void DHudp::readDatagram()
{
    while (i_udpDataSkt->hasPendingDatagrams()) {

        QByteArray dgm;
        QHostAddress sender;
        quint16 senderPort;

        dgm.resize(i_udpDataSkt->pendingDatagramSize());
        i_udpDataSkt->readDatagram(dgm.data(),
                                      dgm.size(),
                                      &sender,
                                      &senderPort);

        Packet p;
        if( p.fromPacket(dgm)){
            enqueueIncomingData(p.getData());
        }
    }
}

void DHudp::sendCmdNext()
{
    this->writeOutCmd(CON_NEXT);
}

void DHudp::sendCmdToCyc(quint32 cyc)
{
    this->writeOutCmd(CON_CHG_CYC,
                      QByteArray(
                          QVariant((qulonglong)cyc).toByteArray()));
}

void DHudp::onIncomingTcpCmdConnection()
{
    if( i_tcpCmdServer->hasPendingConnections()){
        i_tcpCmdSkt = i_tcpCmdServer->nextPendingConnection();
        qDebug() << "DHudp::onIncomingTcpCmdConnection()";
        //TODO check incoming identity
        connect(i_tcpCmdSkt, SIGNAL(readyRead()),
                this, SLOT(onCmdSktReadyRead()));
        connect(i_tcpCmdSkt, SIGNAL(disconnected()),
                this, SLOT(onCmdSktDisconnected()));

        emit sig_cmdConnected();
    }
}

void DHudp::writeOutCmd(quint16 cmd, const QByteArray &arg)
{
    if(!i_tcpCmdSkt) return;

    Packet p(cmd,arg);
    i_tcpCmdSkt->write(p.genPacket());
}

void DHudp::processCMD(const Packet &p)
{
    i_cmd_counter++;

    switch(p.getCMD()){
    case ALA_LAST_CYCLE:
        psCmdDbg("ALA_LAST_CYCLE");
        break;
    case ALA_OUTRANGE_CYC:
        psCmdDbg("ALA_OUTRANGE_CYC","TODO");
        break;
    case ACK_DECODE_PARAM:
        psCmdDbg("ACK_DECODE_PARAM");
        if(p.getCMDarg().size() != 0){
            //prepare decoder
            DecParams param;
            param.fromArray(p.getCMDarg());
            i_decoder->setDecodeParameters(param);
            qDebug() << param.dbgString();

            //send start
            QByteArray arg;
            QDataStream out(&arg, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_8);
            out << i_ipAddress;
            out << (quint16) i_udpDataSkt->localPort();

            this->writeOutCmd(CON_START,arg);
        }
        break;
    case QUE_DATA_PORT:
        psCmdDbg("QUE_DATA_PORT","TODO");
        break;
    default:
        psCmdDbg(QString::number(p.getCMD()) + "?UNKNOWN" );
    }
}

QString DHudp::psCmdDbg(QString cmd, QString arg)
{
    QString dbg;
    dbg = " [DHtcp] cmd " + QString::number(i_cmd_counter);
    dbg += " [" + cmd + "] ";
    dbg += arg;
    dbg += "\tfrom " + i_tcpCmdSkt->peerAddress().toString()
            + " time " + QTime::currentTime().toString();

    qDebug() << dbg;
    return dbg;
}

void DHudp::processData(const Packet &p)
{
    qDebug() << "TODO:  DHudp::processData() ";
}

bool DHudp::startListenData()
{
    i_udpDataSkt->abort();

    if( !i_udpDataSkt->bind(0,QUdpSocket::DontShareAddress) ){
        qDebug() << "\t Err: listen UDP port failed";
        return false;
    }

    connect(i_udpDataSkt,SIGNAL(readyRead()),
            this,SLOT(readDatagram()));

    return true;
}

void DHudp::enqueueIncomingData(const QByteArray &a)
{
    i_queue->waitForEnqueue(a);
    i_procQueueDelayTimer->start(DECODE_QUEUE_DELAY_TIMEOUT);
}

}
