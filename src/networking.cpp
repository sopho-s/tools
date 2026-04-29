#include "networking.h"

RawSocket::RawSocket(const std::string &interface) {
    this->fd = socket(AF_PACKET, SOCK_RAW, htons(0));
    if (this->fd < 0) {
        throw SocketFailedToOpen("Could not open socket");
    }

    ifreq ifr;
    std::memcpy(ifr.ifr_name, interface.c_str(), interface.size());
    int err = ioctl(this->fd, SIOCGIFINDEX, &ifr);
    if (err) {
        throw NoIntefaceIndex("Could not get interface index");
    }
    
    struct sockaddr_ll addr;
    memset(&addr, 0, sizeof(addr));
    addr.sll_family = AF_PACKET;
    addr.sll_protocol = htons(0);
    addr.sll_ifindex = ifr.ifr_ifindex;
    err = bind(this->fd, (struct sockaddr *)(&addr), sizeof(addr));
    if (err) {
        throw BindError("Failed to bind to socket");
    }
}

RawSocket::~RawSocket() {
    close(this->fd);
}

std::vector<unsigned char> RawSocket::ReceivePacketRaw() {
    std::vector<unsigned char> packet(ETH_FRAME_LEN);
    ssize_t size = recv(this->fd, packet.data(), packet.size(), MSG_TRUNC);
    if (size < 0) {
        throw PacketReceiveError("Failed to recieve packets");
    } else {
        packet.resize(size);
    }
    return packet;
}

EthernetFrame RawSocket::ReceivePacket() {
    std::vector<unsigned char> packet(ETH_FRAME_LEN);
    ssize_t size = recv(this->fd, packet.data(), packet.size(), MSG_TRUNC);
    if (size < 0) {
        throw PacketReceiveError("Failed to recieve packets");
    } else {
        packet.resize(size);
    }
    EthernetFrame eth;
    std::memcpy(&eth, packet.data(), sizeof(EthernetFrame)-1);
    eth.data = new unsigned char[packet.size() - (sizeof(EthernetFrame) - 1)];
    std::memcpy(eth.data, packet.data()+sizeof(EthernetFrame)-1, packet.size() - (sizeof(EthernetFrame) - 1));
    return eth;
}


void RawSocket::SendPacketRaw(const std::vector<unsigned char> &packet) {
    ssize_t size = send(this->fd, packet.data(), packet.size(), 0);
    if (size < 0) {
        throw PacketSendError("Failed to send packet");
    } else if (packet.size() != size) {
        throw PacketSendError("Packet truncated");
    }
}

void RawSocket::SendPacket(const EthernetFrame &eth) {
    std::vector<unsigned char> packet(ETH_FRAME_LEN);
    std::memcpy(packet.data(), &eth, sizeof(EthernetFrame) - 1);
    std::memcpy(packet.data()+sizeof(EthernetFrame) - 1, eth.data, ETH_FRAME_LEN - sizeof(EthernetFrame) + 1);
    ssize_t size = send(this->fd, packet.data(), packet.size(), 0);
    if (size < 0) {
        throw PacketSendError("Failed to send packet");
    } else if (packet.size() != size) {
        throw PacketSendError("Packet truncated");
    }
}

IPPacket EthernetFrame::GetIP() {
    IPPacket ret;
    std::memcpy(&ret, this->data, sizeof(IPPacket) - 1);
    ret.data = new unsigned char[(uint16_t)ret.length - (sizeof(IPPacket) - 1)];
    std::memcpy(ret.data, this->data + (sizeof(IPPacket) - 1), (uint16_t)ret.length - (sizeof(IPPacket) - 1));
    return ret;
} 