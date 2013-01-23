#include "dhudp.h"
namespace nProtocUDP{
DHudp::DHudp(QObject *parent) :
    DataHandler(parent),i_tcpCmdServer(0),i_tcpCmdSkt(0),
    i_cmd_counter(0),i_cmdPacketSize(0),i_dataPacketSize(0),
    i_decoder(0)
{
    qDebug() << "DHudp::DHudp()";
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
}

void DHudp::abortWorks()
{
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
            processData(p);
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
    case CON_CONNECTING:
        psCmdDbg("CON_CONNECTING");
        writeOutCmd(CON_CONNECTED);
        break;
    case CON_START:
        psCmdDbg("CON_START","TODO");
        break;
    case CON_NEXT:
        psCmdDbg("CON_NEXT","TODO");
        break;
    case CON_CHG_CYC:
        psCmdDbg("CON_CHG_CYC",
                 QString::number(QVariant(p.getCMDarg()).toUInt()));
        break;
    case CON_START_REPEAT:
        psCmdDbg("CON_START_REPEAT","TODO");
        break;
    case CON_STOP_REPEAT:
        psCmdDbg("CON_STOP_REPEAT","TODO");
        break;
    case CON_DECODE_PARAM_REQ:
        psCmdDbg("CON_DECODE_PARAM_REQ","TODO");
        break;
    case CON_BLOCKS_NUM_REQ:
        psCmdDbg("CON_BLOCKS_NUM_REQ","TODO");
        break;
    case DATA_PORT_DECLARE:
        psCmdDbg("DATALINK_PORT_DECLARE","TODO");
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

}
