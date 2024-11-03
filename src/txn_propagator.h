#ifndef BITCOIN_NET_TXN_PROPAGATOR_H
#define BITCOIN_NET_TXN_PROPAGATOR_H

#include "txn_sending_details.h"
#include "serialize.h"
#include <vector>
#include <chrono>

class CTxnSendingDetails {
private:
    CInv inv;
    CTransactionRef tx;

public:
    CTxnSendingDetails() = default;
    CTxnSendingDetails(const CInv& inv_, const CTransactionRef& tx_) 
        : inv(inv_), tx(tx_) {}

    const CInv& GetInv() const { return inv; }
    const CTransactionRef& GetTx() const { return tx; }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(inv);
        READWRITE(REF(*const_cast<CTransaction*>(tx.get())));
    }
};

class CTxnPropagator {
public:
    virtual ~CTxnPropagator() = default;
    virtual void Shutdown() = 0;
    virtual void AddTransaction(const CTxnSendingDetails& txn) = 0;
    virtual void RemoveTransactions(const std::vector<CTxnSendingDetails>& txns) = 0;
    virtual std::chrono::milliseconds getRunFrequency() const = 0;
    virtual void setRunFrequency(const std::chrono::milliseconds& freq) = 0;
    virtual size_t getNewTxnQueueLength() const = 0;
};

#endif


