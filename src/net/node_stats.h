#ifndef BITCOIN_NET_NODE_STATS_H
#define BITCOIN_NET_NODE_STATS_H

#include <string>
#include "netaddress.h"
#include "net/association.h"

struct NodeStats {
    NodeId id;
    AssociationStats associationStats;
    uint64_t nServices;
    bool fRelayTxes;
    bool fPauseSend;
    bool fUnpauseSend;
    bool fAuthConnEstablished;
    int64_t nTimeConnected;
    int64_t nTimeOffset;
    int nVersion;
    std::string cleanSubVer;
    bool fInbound;
    bool fAddnode;
    int nStartingHeight;
    bool fWhitelisted;
    double dPingTime;
    double dMinPing;
    double dPingWait;
    CService addrLocal;
    size_t nInvQueueSize;
};

#endif // BITCOIN_NET_NODE_STATS_H
