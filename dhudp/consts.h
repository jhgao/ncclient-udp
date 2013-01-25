#ifndef CONSTS_H
#define CONSTS_H

namespace nProtocUDP{
static const int ONE_CYCLE_BLOCKS = 5;  // how many block to send in one cycle

static const int PACKET_SIZE = 512; //UDP datagram 512k for internet transfer, data payload

static const int FRAGMENT_SIZE = PACKET_SIZE; // let fragments size equals to packet size

static const int ENC_BLOCK_SIZE = (10*1024);  //QNCencoder
}//namespace nProtocUDP
#endif // CONSTS_H
