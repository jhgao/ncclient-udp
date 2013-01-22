/*          quit16              payloadLength
 *          <payloadLength>      <payload>
 *  payload = type + cmd
 *  or      = type + data
 */

#ifndef PACKET_H
#define PACKET_H

#include <QtNetwork>
#include <QDataStream>

enum ePacketType{
    PTYPE_DATA,
    PTYPE_CMD
};

class Packet
{
public:
    explicit Packet(quint16 cmd,
                    const QByteArray arg = QByteArray()); //cmd packet

    explicit Packet(const QByteArray &data); //data packet

    explicit Packet();

    const QByteArray& genPacket();

    bool fromPacket(QByteArray a);
    bool fromPayload(QByteArray a);

    quint16 getType() const;
    quint16 getCMD() const;
    QByteArray getCMDarg() const;
    QByteArray getData()const;

    QString dbgString()const;
private:
    quint16 i_type;

    quint16     i_cmd;
    QByteArray  i_cmd_arg;

    QByteArray i_data;

    QByteArray i_payload;
    QByteArray i_packet;
};

#endif // PACKET_H
