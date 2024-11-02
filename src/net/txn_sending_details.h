#ifndef BITCOIN_NET_TXN_SENDING_DETAILS_H
#define BITCOIN_NET_TXN_SENDING_DETAILS_H

#include "primitives/transaction.h"
#include "protocol.h"
#include "txmempool.h"

class CTxnSendingDetails {
public:
    CInv inv;
    CTransactionRef tx;
    TxMempoolInfo info;

    const CInv& GetInv() const { return inv; }
    const CTransactionRef& GetTx() const { return tx; }
    const TxMempoolInfo& GetInfo() const { return info; }
    uint256 GetHash() const { return tx->GetHash(); }
};

#endif 