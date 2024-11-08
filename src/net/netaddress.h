// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_NETADDRESS_H
#define BITCOIN_NETADDRESS_H

#if defined(HAVE_CONFIG_H)
#include "config/bitcoin-config.h"
#endif

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

/** IP address (IPv6, or IPv4 using mapped IPv6 range (::FFFF:0:0/96)) */
class CNetAddr {
protected:
    uint8_t ip[16] = {0};
    uint32_t scopeId{0};

public:
    CNetAddr() = default;
    CNetAddr(const struct in_addr& ipv4Addr);
    explicit CNetAddr(const struct in6_addr& ipv6Addr);
    explicit CNetAddr(const char* pszIp, bool fAllowLookup = false);

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

    bool IsIPv4() const;
    bool IsIPv6() const;
    bool IsRFC1918() const;
    bool IsRFC2544() const;
    bool IsRFC6598() const;
    bool IsRFC5737() const;
    bool IsRFC3849() const;
    bool IsRFC3927() const;
    bool IsRFC3964() const;
    bool IsRFC4193() const;
    bool IsRFC4380() const;
    bool IsRFC4843() const;
    bool IsRFC4862() const;
    bool IsRFC6052() const;
    bool IsRFC6145() const;
    bool IsLocal() const;
    bool IsRoutable() const;
    bool IsValid() const;
    bool IsMulticast() const;
    
    std::vector<uint8_t> GetGroup() const;
    std::string ToString() const;
    std::string ToStringIP() const;
    
    friend bool operator==(const CNetAddr& a, const CNetAddr& b);
    friend bool operator!=(const CNetAddr& a, const CNetAddr& b);
    friend bool operator<(const CNetAddr& a, const CNetAddr& b);
};

/** A combination of a network address (CNetAddr) and a (TCP) port */
class CService : public CNetAddr {
protected:
    uint16_t port{0};

public:
    CService() = default;
    CService(const CNetAddr& ip, unsigned short port);
    CService(const struct in_addr& ipv4Addr, unsigned short port);
    CService(const struct in6_addr& ipv6Addr, unsigned short port);
    explicit CService(const struct sockaddr_in& addr);
    explicit CService(const struct sockaddr_in6& addr);

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

    unsigned short GetPort() const { return port; }
    void SetPort(unsigned short portIn) { port = portIn; }
    
    std::string ToString() const;
    std::string ToStringPort() const;
    std::string ToStringIPPort() const;
};

#endif // BITCOIN_NETADDRESS_H
