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
    ~IPv4Packet() {
        delete this->data;
    }
    std::string ToString();
};

struct EthernetFrame {
    unsigned char dest[6];
    unsigned char source[6];
    unsigned char lengthtype[2];
    unsigned char *data;

    ~EthernetFrame() {
        delete[] this->data;
    }
    void ParseVec(const std::vector<unsigned char> frame);
    IPv4Packet GetIPv4();
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