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
    std::reverse(eth.source, eth.source + 5);
    std::reverse(eth.dest, eth.dest + 5);
    eth.truesize = size;
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
    std::memcpy(packet.data(), &eth, SIZEOFETH);
    std::memcpy(packet.data()+SIZEOFETH, eth.data, ETH_FRAME_LEN - SIZEOFETH + 1);
    ssize_t size = send(this->fd, packet.data(), packet.size(), 0);
    if (size < 0) {
        throw PacketSendError("Failed to send packet");
    } else if (packet.size() != size) {
        throw PacketSendError("Packet truncated");
    }
}

IPv4Packet EthernetFrame::GetIPv4() const {
    IPv4Packet ret;
    std::memcpy(&ret, this->data, SIZEOFIPV4);
    ret.data = new unsigned char[ret.length - (SIZEOFIPV4)];
    std::memcpy(ret.data, this->data + (SIZEOFIPV4), ret.length - (SIZEOFIPV4));
    ret.source = htonl(ret.source);
    ret.dest = htonl(ret.dest);
    ret.truesize = this->truesize - SIZEOFETH;
    return ret;
} 

TCPPacket IPv4Packet::GetTCP() const {
    TCPPacket ret;
    std::memcpy(&ret, this->data, SIZEOFTCP);
    ret.offset = ret.offset >> 4;
    ret.options = new unsigned char[ret.offset - (SIZEOFTCP)];
    std::memcpy(ret.options, this->data + (SIZEOFTCP), ret.offset - (SIZEOFTCP));
    ret.truesize = this->truesize - SIZEOFIPV4;
    if (ret.truesize < ret.offset) {
        throw NetworkSizeMismatch("offset is larger than the size of the data");
    }
    uint16_t remainingsize = ret.truesize - ret.offset;
    ret.options = new unsigned char[remainingsize];
    std::memcpy(ret.data, this->data + ret.offset, remainingsize - (SIZEOFTCP));
    ret.source = htons(ret.source);
    ret.dest = htons(ret.dest);
    return ret;
} 

std::string EthernetFrame::ToString() {
    std::string ret = "";
    ret += "destination mac: " + ToHexString(this->dest, 6) + "\n";
    ret += "source mac: " + ToHexString(this->source, 6) + "\n";
    ret += "length or type: " + ToHexString(this->lengthtype, 2) + "\n";
    return ret;
} 

std::string IPv4Packet::ToString() {
    std::string ret = "";
    ret += "version and ihl: " + ToHexString(this->vihl) + "\n";
    ret += "tos: " + ToHexString(this->tos) + "\n";
    ret += "length: " + ToHexString(this->length) + "\n";
    ret += "identification: " + ToHexString(this->identification, 2) + "\n";
    ret += "flags and frag offset: " + ToHexString(this->flagfrag) + "\n";
    ret += "ttl: " + ToHexString(this->ttl) + "\n";
    ret += "protocol: " + ToHexString(this->protocol) + "\n";
    ret += "checksum: " + ToHexString(this->checksum, 2) + "\n";
    ret += "source: " + ToIPString(this->source) + "\n";
    ret += "destination: " + ToIPString(this->dest) + "\n";
    ret += "options: " + ToHexString(this->options, 40) + "\n";
    return ret;
} 

void EthernetFrame::ParseVec(std::vector<unsigned char> frame) {
    std::memcpy(this, frame.data(), SIZEOFETH);
    this->data = new unsigned char[frame.size() - (SIZEOFETH)];
    std::memcpy(this->data, frame.data()+SIZEOFETH, frame.size() - (SIZEOFETH));
    this->truesize = frame.size();
}