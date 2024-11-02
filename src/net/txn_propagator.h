#ifndef BITCOIN_NET_TXN_PROPAGATOR_H
#define BITCOIN_NET_TXN_PROPAGATOR_H

#include "txn_sending_details.h"
#include <vector>

class CTxnPropagator {
public:
    virtual ~CTxnPropagator() = default;
    virtual void Shutdown() = 0;
    virtual void AddTransaction(const CTxnSendingDetails& txn) = 0;
    virtual void RemoveTransactions(const std::vector<CTxnSendingDetails>& txns) = 0;
};

#endif 