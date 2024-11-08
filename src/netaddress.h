#ifndef BITCOIN_NETADDRESS_H
#define BITCOIN_NETADDRESS_H

#include "compat.h"
#include "serialize.h"

#include <cstdint>
#include <string>
#include <vector>

enum Network {
    NET_UNROUTABLE = 0,
    NET_IPV4,
    NET_IPV6,
    NET_MAX,
};

class CNetAddr {
protected:
    uint8_t ip[16]{};
    uint32_t scopeId{0};

public:
    CNetAddr() = default;
    explicit CNetAddr(const struct in_addr& ipv4Addr);
    void SetIP(const CNetAddr& ip);

    template<typename Stream>
    void Serialize(Stream& s) const {
        s.write(reinterpret_cast<const char*>(ip), sizeof(ip));
        s << scopeId;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s.read(reinterpret_cast<char*>(ip), sizeof(ip));
        s >> scopeId;
    }

    // ... (keep existing methods) ...
};

class CSubNet {
protected:
    CNetAddr network;
    uint8_t netmask[16]{};
    bool valid{false};

public:
    CSubNet() = default;
    CSubNet(const CNetAddr& addr, int32_t mask);
    CSubNet(const CNetAddr& addr, const CNetAddr& mask);

    template<typename Stream>
    void Serialize(Stream& s) const {
        s << network;
        s.write(reinterpret_cast<const char*>(netmask), sizeof(netmask));
        s << valid;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> network;
        s.read(reinterpret_cast<char*>(netmask), sizeof(netmask));
        s >> valid;
    }

    // ... (keep existing methods) ...
};

class CService : public CNetAddr {
protected:
    uint16_t port{0};

public:
    CService() = default;
    CService(const CNetAddr& ip, uint16_t port);
    CService(const struct in_addr& ipv4Addr, uint16_t port);
    CService(const struct sockaddr_in& addr);
    CService(const struct in6_addr& ipv6Addr, uint16_t port);
    CService(const struct sockaddr_in6& addr);

    template<typename Stream>
    void Serialize(Stream& s) const {
        CNetAddr::Serialize(s);
        s << port;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        CNetAddr::Unserialize(s);
        s >> port;
    }

    // ... (keep existing methods) ...
};

#endif // BITCOIN_NETADDRESS_H 