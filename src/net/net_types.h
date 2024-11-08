#ifndef BITCOIN_NET_NET_TYPES_H
#define BITCOIN_NET_NET_TYPES_H

#include "netaddress.h"
#include "protocol.h"
#include "serialize.h"
#include "uint256.h"
#include <atomic>
#include <cstdint>
#include <memory>
#include <string>

class CNode;
using CNodePtr = std::shared_ptr<CNode>;

/** Different types of network connections we support */
enum class ConnectionType : uint8_t {
    NONE = 0,
    INBOUND = 1,
    OUTBOUND = 2,
    MANUAL = 3,
    FEELER = 4,
    BLOCK_RELAY = 5
};

/** Information about a peer */
class CNodeStats {
public:
    NodeId nodeid;
    ServiceFlags nServices;
    bool fRelayTxes;
    int64_t nLastSend;
    int64_t nLastRecv;
    int64_t nTimeConnected;
    int64_t nTimeOffset;
    std::string addrName;
    int nVersion;
    std::string cleanSubVer;
    bool fInbound;
    bool fAddnode;
    int nStartingHeight;
    uint64_t nSendBytes;
    uint64_t nRecvBytes;
    bool fWhitelisted;
    double dPingTime;
    double dPingWait;
    double dMinPing;
    Amount minFeeFilter;
    std::string addrLocal;
    CAddress addr;

    template<typename Stream>
    void Serialize(Stream& s) const {
        s << nodeid;
        s << nServices;
        s << fRelayTxes;
        s << nLastSend;
        s << nLastRecv;
        s << nTimeConnected;
        s << nTimeOffset;
        s << addrName;
        s << nVersion;
        s << cleanSubVer;
        s << fInbound;
        s << fAddnode;
        s << nStartingHeight;
        s << nSendBytes;
        s << nRecvBytes;
        s << fWhitelisted;
        s << dPingTime;
        s << dPingWait;
        s << dMinPing;
        s << minFeeFilter;
        s << addrLocal;
        s << addr;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> nodeid;
        s >> nServices;
        s >> fRelayTxes;
        s >> nLastSend;
        s >> nLastRecv;
        s >> nTimeConnected;
        s >> nTimeOffset;
        s >> addrName;
        s >> nVersion;
        s >> cleanSubVer;
        s >> fInbound;
        s >> fAddnode;
        s >> nStartingHeight;
        s >> nSendBytes;
        s >> nRecvBytes;
        s >> fWhitelisted;
        s >> dPingTime;
        s >> dPingWait;
        s >> dMinPing;
        s >> minFeeFilter;
        s >> addrLocal;
        s >> addr;
    }
};

#endif // BITCOIN_NET_NET_TYPES_H
