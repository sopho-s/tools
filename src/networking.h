#include <sys/socket.h>
#include <stdint.h>
#include <string>
#include <arpa/inet.h>
#include "exceptions.h"
#include "util.h"
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <vector>
#pragma once

#define ETHER_TYPE_IPv4 0x0800 /**< IPv4 Protocol. */
#define ETHER_TYPE_IPv6 0x86DD /**< IPv6 Protocol. */
#define ETHER_TYPE_ARP  0x0806 /**< Arp Protocol. */
#define ETHER_TYPE_RARP 0x8035 /**< Reverse Arp Protocol. */
#define ETHER_TYPE_VLAN 0x8100 /**< IEEE 802.1Q VLAN tagging. */
#define ETHER_TYPE_1588 0x88F7 /**< IEEE 802.1AS 1588 Precise Time Protocol. */
#define ETHER_TYPE_SLOW 0x8809 /**< Slow protocols (LACP and Marker). */
#define ETHER_TYPE_TEB  0x6558 /**< Transparent Ethernet Bridging. */

#define SIZEOFETH 14
#define SIZEOFIPV4 24
#define SIZEOFTCP 20

#define LOCALDEFAULT (uint32_t)0x7f000001
#define RESERVED1 (uint32_t)0x00000000
#define RESERVED1RANGE 8
#define RESERVED2 (uint32_t)0xa9fe0000
#define RESERVED2RANGE 16
#define RESERVED3 (uint32_t)0xf0000000
#define RESERVED3RANGE 4

struct TCPPacket {
    uint16_t source;
    uint16_t dest;
    uint32_t sequencenumber;
    uint32_t acknumber;
    unsigned char offset;
    unsigned char flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgentptr;
    unsigned char *options;
    unsigned char *data;
    uint32_t truesize;
    ~TCPPacket() {
        delete[] this->options;
        delete[] this->data;
    }
    std::string ToString();
};

struct IPv4Packet {
    unsigned char vihl;
    unsigned char tos;
    uint16_t length;
    unsigned char identification[2];
    uint16_t flagfrag;
    unsigned char ttl;
    unsigned char protocol;
    unsigned char checksum[2];
    uint32_t source;
    uint32_t dest;
    unsigned char options[40];
    unsigned char *data;
    uint32_t truesize;
    ~IPv4Packet() {
        delete[] this->data;
    }
    TCPPacket GetTCP() const;
    std::string ToString();
};

struct EthernetFrame {
    unsigned char dest[6];
    unsigned char source[6];
    unsigned char lengthtype[2];
    unsigned char *data;
    uint32_t truesize;
    ~EthernetFrame() {
        delete[] this->data;
    }
    void ParseVec(const std::vector<unsigned char> frame);
    IPv4Packet GetIPv4() const;
    std::string ToString();
};

class RawSocket {
    private:
        int32_t fd;
    public:
        RawSocket();
        RawSocket(const std::string &interface);
        ~RawSocket();
        std::vector<unsigned char> ReceivePacketRaw();
        EthernetFrame ReceivePacket();
        void SendPacketRaw(const std::vector<unsigned char> &packet);
        void SendPacket(const EthernetFrame &eth);
};