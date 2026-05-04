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

struct IPPacket {
    union {
        unsigned char version : 4;
        unsigned char ihl : 4;
    } vihl;
    unsigned char tos;
    uint16_t length;
    unsigned char identification[2];
    union {
        unsigned char flags : 3;
        uint16_t fragoffset : 13;
    } flagfrag;
    unsigned char ttl;
    unsigned char protocol;
    unsigned char checksum[2];
    unsigned char source[4];
    unsigned char dest[4];
    unsigned char options[40];
    unsigned char *data;
    ~IPPacket() {
        delete this->data;
    }
    std::string ToString();
};

struct EthernetFrame {
    unsigned char preamble[7];
    unsigned char sfd;
    unsigned char dest[6];
    unsigned char source[6];
    uint32_t lengthtype[2];
    unsigned char *data;
    unsigned char crc[4];

    ~EthernetFrame() {
        delete this->data;
    }
    void ParseVec(std::vector<unsigned char> frame);
    IPPacket GetIP();
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