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

    template<typename Stream>
    void Serialize(Stream& s) const {
        s << inv;
        s << *tx;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> inv;
        tx = MakeTransactionRef();
        s >> *tx;
    }
};

// Vector serialization helper for CTxnSendingDetails
template<typename Stream>
void SerializeVector(Stream& s, const std::vector<CTxnSendingDetails>& v) {
    WriteCompactSize(s, v.size());
    for(const auto& item : v) {
        ::Serialize(s, item);
    }
}

template<typename Stream>
void UnserializeVector(Stream& s, std::vector<CTxnSendingDetails>& v) {
    v.clear();
    size_t size = ReadCompactSize(s);
    v.reserve(size);
    for(size_t i = 0; i < size; i++) {
        CTxnSendingDetails item;
        ::Unserialize(s, item);
        v.push_back(item);
    }
}

class CTxnPropagator {
public:
    static constexpr unsigned DEFAULT_RUN_FREQUENCY_MILLIS {1000};

    virtual ~CTxnPropagator() = default;
    virtual void Shutdown() = 0;
    virtual void AddTransaction(const CTxnSendingDetails& txn) = 0;
    virtual void RemoveTransactions(const std::vector<CTxnSendingDetails>& txns) = 0;
    virtual std::chrono::milliseconds getRunFrequency() const = 0;
    virtual void setRunFrequency(const std::chrono::milliseconds& freq) = 0;
    virtual size_t getNewTxnQueueLength() const = 0;
};

#endif // BITCOIN_NET_TXN_PROPAGATOR_H 