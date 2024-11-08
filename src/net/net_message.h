// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2017 The Bitcoin developers
// Copyright (c) 2020-2021 Bitcoin Association
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef BITCOIN_NET_NET_MESSAGE_H
#define BITCOIN_NET_NET_MESSAGE_H

#include "hash.h"
#include "net/net_message_header.h"
#include "serialize.h"
#include "streams.h"
#include "uint256.h"

#include <array>
#include <cstdint>
#include <string>

class Config;

class CNetMessage {
private:
    mutable CHash256 hasher;
    mutable uint256 data_hash;
    CMessageHeader hdr;
    CDataStream dataBuff;

public:
    CNetMessage(const CMessageHeader::MessageMagic& pchMessageStartIn)
        : hdr(pchMessageStartIn), dataBuff(SER_NETWORK, INIT_PROTO_VERSION) {}

    uint64_t Read(const Config& config, const char* pch, uint64_t nBytes);
    uint64_t Read(const Config& config, const uint8_t* pch, uint64_t nBytes);

    const CMessageHeader& GetHeader() const { return hdr; }
    bool Complete() const { return hdr.Complete() && dataBuff.size() == hdr.GetMessageSize(); }
    const uint256& GetMessageHash() const;
    uint64_t GetTotalLength() const;
    CDataStream& GetData() { return dataBuff; }

    template<typename Stream>
    void Serialize(Stream& s) const {
        s << hdr;
        s << dataBuff;
        s << data_hash;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> hdr;
        s >> dataBuff;
        s >> data_hash;
    }
};

#endif // BITCOIN_NET_NET_MESSAGE_H

