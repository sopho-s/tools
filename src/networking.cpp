#include "networking.h"

RawSocket::RawSocket() {
    this->fd = -1;
}

RawSocket::RawSocket(const std::string &interface) {
    this->fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (this->fd < 0) {
        throw SocketFailedToOpen("Could not open socket");
    }

    ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    if (interface.size() >= IFNAMSIZ) {
        throw NoInterfaceIndex("Interface name too long");
    }
    std::memcpy(ifr.ifr_name, interface.c_str(), interface.size()+1);
    int err = ioctl(this->fd, SIOCGIFINDEX, &ifr);
    if (err) {
        throw NoInterfaceIndex("Could not get interface index");
    }
    
    struct sockaddr_ll addr;
    memset(&addr, 0, sizeof(addr));
    addr.sll_family = AF_PACKET;
    addr.sll_protocol = htons(ETH_P_ALL);
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
    ret.data = new unsigned char[ret.length - (sizeof(IPPacket) - 1)];
    std::memcpy(ret.data, this->data + (sizeof(IPPacket) - 1), ret.length - (sizeof(IPPacket) - 1));
    return ret;
} 

std::string EthernetFrame::ToString() {
    std::string ret = "";
    ret += "preamble: " + ToHexString(this->preamble, 7) + "\n";
    ret += "sfd: " + ToHexString(&(this->sfd), 1) + "\n";
    ret += "destination mac: " + ToHexString(this->dest, 6) + "\n";
    ret += "source mac: " + ToHexString(this->source, 6) + "\n";
    ret += "crc: " + ToHexString(this->crc, 4) + "\n";
    return ret;
} 

void EthernetFrame::ParseVec(std::vector<unsigned char> frame) {
    std::memcpy(this, frame.data(), sizeof(EthernetFrame)-1);
    this->data = new unsigned char[frame.size() - (sizeof(EthernetFrame) - 1)];
    std::memcpy(this->data, frame.data()+sizeof(EthernetFrame)-1, frame.size() - (sizeof(EthernetFrame) - 1));
}