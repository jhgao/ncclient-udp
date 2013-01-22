/*
 * (a UDP data transfer protocol definition)
 *
 *  Server send data block by cycle, one cycle multiple data blocks.
 *
 *  client trigger next cycle when received all block of a cycle,
 *  client trigger next cycle by sending command via TCP to server.
 */

#ifndef CMD_DEFINE_H
#define CMD_DEFINE_H

enum eControl_CMD {
    // for link
    CON_CONNECTING,
    CON_CONNECTED,
    CON_START,
    CON_NEXT,
    CON_CHG_CYC, //jump to certain cycle
    CON_D5F,
    CON_D5FACK,

    CON_START_REPEAT,   //[client]start a continuous repeat sending of current cycle data
    CON_STOP_REPEAT,
    ALA_LAST_CYCLE,  //[server] reached the last cycle, arg blocks number in last cycle
    ALA_OUTRANGE_CYC,

    //data link negotiate (note these are commands too)
    DATALINK_PROTOCOL_TCP,    //Data transfer type
    DATALINK_PROTOCOL_TCP_ACK,
    DATALINK_PROTOCOL_UDP,
    DATALINK_PROTOCOL_UDP_ACK,
    DATALINK_PORT_QUERY,  //question remote data port
    DATALINK_PORT_DECLARE,   //declare self listing data port
    DATALINK_PORT_DECLARE_ACK,  //acknowledgement of GOT a port declare
    //new
    DATALINK_DECLARE,   //with arguments (protocol, protoc args)
    DATALINK_DECLARE_ACK,   //with arguments (protocol, ack args)

    //encoding & decoding parameter negotiate
    CON_DECODE_PARAM_REQ,   //[client] request this to initialize decoder
    CON_DECODE_PARAM_ACK,   //[server] replay this to client

    CON_BLOCKS_NUM_REQ, //old
    CON_BLOCKS_NUM_ACK, //old
    CON_FILE_SIZE_REQ,  //old[client] tell me total file size please
    CON_FILE_SIZE_ACK  //old[server] current sending file size
};

#endif // CMD_DEFINE_H
