#ifndef DHUDPPROTOCOL_H
#define DHUDPPROTOCOL_H

namespace nProtocUDP{

enum eControl_CMD {
    // client
    CON_START,  //with arg(client ip, data port) request UDP data start
    CON_NEXT,
    CON_CHG_CYC, //jump to certain cycle
    ALA_DONE,   //client alert: received all file

    // server
    ALA_LAST_CYCLE,  //[server] reached the last cycle, arg blocks number in last cycle
    ALA_OUTRANGE_CYC,

    //encoding & decoding parameter negotiate
    QUE_DECODE_PARAM,   //[client] request this to initialize decoder
    ACK_DECODE_PARAM,   //[server] replay this to client

    //data link negotiate (note these are commands too)
    QUE_DATA_PORT,  //question remote data port
    ACK_DATA_PORT   //declare self listing data port

};
}//namespace nProtocUDP
#endif // DHUDPPROTOCOL_H
