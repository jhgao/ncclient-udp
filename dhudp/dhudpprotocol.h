#ifndef DHUDPPROTOCOL_H
#define DHUDPPROTOCOL_H

namespace nProtocUDP{

static const int ONE_CYCLE_BLOCKS = 5;  // how many block to send in one cycle

static const int PACKET_SIZE = 512; //UDP datagram 512k for internet transfer, data payload

static const int FRAGMENT_SIZE = PACKET_SIZE; // let fragments size equals to packet size

static const int SEND_FRAGMENT_INTERVAL = 2; //msec

static const int ENC_BLOCK_SIZE = (10*1024);  //QNCencoder

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
