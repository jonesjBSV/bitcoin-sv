#ifndef BITCOIN_NET_NET_H
#define BITCOIN_NET_NET_H

#include "addrdb.h"
#include "addrman.h"
#include "amount.h"
#include "bloom.h"
#include "compat.h"
#include "fs.h"
#include "hash.h"
#include "limitedmap.h"
#include "net/net_types.h"
#include "netaddress.h"
#include "protocol.h"
#include "random.h"
#include "streams.h"
#include "sync.h"
#include "threadinterrupt.h"
#include "uint256.h"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <memory>
#include <thread>
#include <vector>

class CNode;
class CScheduler;
class Config;
class CConnman;

struct CNodeStats {
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

// ... (keep rest of file) ...

#endif // BITCOIN_NET_NET_H
