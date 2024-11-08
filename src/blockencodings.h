// Copyright (c) 2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_BLOCK_ENCODINGS_H
#define BITCOIN_BLOCK_ENCODINGS_H

#include "primitives/block.h"

#include <memory>

class Config;
class CTxMemPool;
class CFileReader;
template<typename Reader>
class CBlockStreamReader;

// Dumb helper to handle CTransaction compression at serialize-time
struct TransactionCompressor {
private:
    CTransactionRef tx;

public:
    TransactionCompressor(const CTransactionRef& tx) : tx(tx) {}
    
    template<typename Stream>
    void Serialize(Stream& s) const {
        CompressedTransaction txn(*tx);
        s << txn;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        CompressedTransaction txn;
        s >> txn;
        tx = txn.GetTx();
    }
};

class BlockTransactionsRequest {
private:
    uint256 blockhash;
    std::vector<uint16_t> indexes;

public:
    template<typename Stream>
    void Serialize(Stream& s) const {
        s << blockhash;
        WriteCompactSize(s, indexes.size());
        for(const uint16_t& index : indexes) {
            s << index;
        }
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> blockhash;
        uint64_t indexes_size = ReadCompactSize(s);
        indexes.resize(indexes_size);
        for(uint16_t& index : indexes) {
            s >> index;
        }
    }
};

class BlockTransactions {
private:
    uint256 blockhash;
    std::vector<CTransactionRef> txn;

public:
    template<typename Stream>
    void Serialize(Stream& s) const {
        s << blockhash;
        WriteCompactSize(s, txn.size());
        for(const auto& tx : txn) {
            s << *tx;
        }
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> blockhash;
        uint64_t txn_size = ReadCompactSize(s);
        txn.resize(txn_size);
        for(size_t i = 0; i < txn_size; i++) {
            txn[i] = MakeTransactionRef();
            s >> *txn[i];
        }
    }
};

size_t ser_size(const BlockTransactions&); 

// Dumb serialization/storage-helper for CBlockHeaderAndShortTxIDs and
// PartiallyDownloadedBlock
struct PrefilledTransaction {
    // Used as an offset since last prefilled tx in CBlockHeaderAndShortTxIDs,
    // as a proper transaction-in-block-index in PartiallyDownloadedBlock
    uint32_t index;
    CTransactionRef tx;

    ADD_SERIALIZE_METHODS

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation ser_action) {
        READWRITE(COMPACTSIZE(index));
        READWRITE(REF(TransactionCompressor(tx)));
    }
};

typedef enum ReadStatus_t {
    READ_STATUS_OK,
    // Invalid object, peer is sending bogus crap.
    // FIXME: differenciate bogus crap from crap that do not fit our policy.
    READ_STATUS_INVALID,
    // Failed to process object.
    READ_STATUS_FAILED,
    // Used only by FillBlock to indicate a failure in CheckBlock.
    READ_STATUS_CHECKBLOCK_FAILED,
} ReadStatus;

class CBlockHeaderAndShortTxIDs {
private:
    mutable uint64_t shorttxidk0, shorttxidk1;
    uint64_t nonce;

    void FillShortTxIDSelector() const;

    friend class PartiallyDownloadedBlock;

    static const int SHORTTXIDS_LENGTH = 6;

protected:
    std::vector<uint64_t> shorttxids;
    std::vector<PrefilledTransaction> prefilledtxn;

public:
    CBlockHeader header;

    // Dummy for deserialization
    CBlockHeaderAndShortTxIDs() {}

    CBlockHeaderAndShortTxIDs(const CBlock &block);
    CBlockHeaderAndShortTxIDs(CBlockStreamReader<CFileReader>& stream);

    uint64_t GetShortID(const uint256 &txhash) const;

    size_t BlockTxCount() const {
        return shorttxids.size() + prefilledtxn.size();
    }

    ADD_SERIALIZE_METHODS

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation ser_action) {
        READWRITE(header);
        READWRITE(nonce);
        size_t shorttxids_size = shorttxids.size();
        READWRITE(VARINT(shorttxids_size));
        shorttxids.resize(shorttxids_size);
        for (size_t i = 0; i < shorttxids.size(); i++) {
            uint32_t lsb = shorttxids[i] & 0xffffffff;
            uint16_t msb = (shorttxids[i] >> 32) & 0xffff;
            READWRITE(lsb);
            READWRITE(msb);
            shorttxids[i] = (uint64_t(msb) << 32) | uint64_t(lsb);
        }
        READWRITE(prefilledtxn);
    }
};

class PartiallyDownloadedBlock {
protected:
    std::vector<CTransactionRef> txns_available;
    size_t prefilled_count = 0, mempool_count = 0, extra_count = 0;
    CTxMemPool *pool;
    const Config *config;

public:
    CBlockHeader header;
    PartiallyDownloadedBlock(const Config &configIn, CTxMemPool *poolIn)
        : pool(poolIn), config(&configIn) {}

    // extra_txn is a list of extra transactions to look at, in <txhash,
    // reference> form.
    ReadStatus
    InitData(const CBlockHeaderAndShortTxIDs &cmpctblock,
             const std::vector<std::pair<uint256, CTransactionRef>> &extra_txn);
    bool IsTxAvailable(size_t index) const;
    ReadStatus FillBlock(CBlock &block,
                         const std::vector<CTransactionRef> &vtx_missing,
                         int32_t blockHeight);
};

#endif
