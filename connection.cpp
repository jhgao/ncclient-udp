#include "connection.h"

Connection::Connection(QObject *parent) :
    QTcpSocket(parent),packetSize(0),i_cmd_counter(0),i_dh(0),
    i_ackProtoc(PROTOC_NONE)
{
    connect(this, SIGNAL(readyRead()),
            this, SLOT(onControlSktReadyRead()));
    connect(this, SIGNAL(connected()),
            this, SLOT(onConnected()));

    //init data handler
    i_dh = new nProtocUDP::DHudp(this);
    connect(i_dh, SIGNAL(sig_progressPercent(uint)),
            this, SIGNAL(sig_progressPercent(uint)));
    connect(i_dh, SIGNAL(sig_gotBlockSN(quint32)),
            this, SIGNAL(sig_gotBlockSN(quint32)));
}

void Connection::onControlSktReadyRead()
{
    packetSize = 0;

    //get packet size
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_8);
    if (packetSize == 0) {
        if (this->bytesAvailable() < (int)sizeof(quint16)){
            qDebug() << "\t E: packet size wrong"
                     << this->bytesAvailable()
                     << "/"
                     << (int)sizeof(quint16);;
            return;
        }
        in >> packetSize;
    }

    //ensure data size available
    if (this->bytesAvailable() < packetSize){
        qDebug() << "\t E: not enough data bytes"
                 << this->bytesAvailable()
                 << "/need "
                 << packetSize;
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
            qDebug() << "\t Connection: Data from CMD link";
            break;
        default:
            qDebug() << "\t Connection: Unknown packet type";
        }
    }
}

void Connection::processCMD(const Packet &p)
{
    i_cmd_counter++;

    QDataStream args(p.getCMDarg());
    args.setVersion(QDataStream::Qt_4_8);

    switch(p.getCMD()){
    case DATALINK_DECLARE_ACK:
        psCmdDbg("DATALINK_DECLARE_ACK");
        args >> i_ackProtoc;
        args >> i_ackProtocArg;
        this->processProtocolAck((eProtocTypes)i_ackProtoc,i_ackProtocArg);
        break;
    default:
        qDebug() << "\t unknown cmd" << p.getCMD();
    }
}

void Connection::writeOutCMD(quint16 cmd, const QByteArray arg)
{
    if(!this->isWritable()) return;

    Packet p(cmd,arg);
    this->write(p.genPacket());
}

void Connection::onConnected()
{
    QByteArray arg;
    QDataStream out( &arg, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (quint16)i_dh->type();
    out << i_dh->declareArg();
    this->writeOutCMD(DATALINK_DECLARE, arg);
}

QString Connection::psCmdDbg(QString cmd, QString arg)
{
    QString dbg;
    dbg = "Client Connection got CMD " + QString::number(i_cmd_counter);
    dbg += " [" + cmd + "] ";
    dbg += arg;
    dbg += "\tfrom " + this->peerAddress().toString()
            + " time " + QTime::currentTime().toString();

    qDebug() << dbg;
    return dbg;
}

void Connection::processProtocolAck(eProtocTypes type, const QByteArray protocArg)
{
    if( PROTOC_TCP == type ){
        //TODO judge protocArg
        QString serverDataSktAddr;
        quint16 serverDataSktPort;

        QDataStream in(protocArg);
        in.setVersion(QDataStream::Qt_4_8);
        in >> serverDataSktAddr;
        in >> serverDataSktPort;

        qDebug() << "Server ACK: TCP protocol, server DATA skt:"
                 << serverDataSktAddr
                 << " : " << serverDataSktPort;
    }
}


void Connection::slot_abort()
{
    i_dh->abortWorks();
    this->abort();
}

void Connection::slot_connectToHost(QString addr, quint16 port)
{
    qDebug() << "Connection::slot_connectToHost()";
    this->connectToHost(QHostAddress(addr), port);
}

