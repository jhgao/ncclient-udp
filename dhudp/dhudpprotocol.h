#ifndef DHUDPPROTOCOL_H
#define DHUDPPROTOCOL_H

namespace nProtocUDP{

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
    DATA_PORT_QUERY,  //question remote data port
    DATA_PORT_DECLARE,   //declare self listing data port

    //encoding & decoding parameter negotiate
    CON_DECODE_PARAM_REQ,   //[client] request this to initialize decoder
    CON_DECODE_PARAM_ACK,   //[server] replay this to client

    CON_BLOCKS_NUM_REQ, //old
    CON_BLOCKS_NUM_ACK, //old
    CON_FILE_SIZE_REQ,  //old[client] tell me total file size please
    CON_FILE_SIZE_ACK  //old[server] current sending file size
};
}//namespace nProtocUDP
#endif // DHUDPPROTOCOL_H
