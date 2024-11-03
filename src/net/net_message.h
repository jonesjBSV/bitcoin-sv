// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2017 The Bitcoin developers
// Copyright (c) 2020-2021 Bitcoin Association
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef BITCOIN_NET_NET_MESSAGE_H
#define BITCOIN_NET_NET_MESSAGE_H

#include "hash.h"
#include "net/net_types.h"
#include "serialize.h"
#include "streams.h"
#include "uint256.h"
#include "utilstrencodings.h"

#include <array>
#include <cstdint>
#include <string>

class Config;
class CNetMessage;

class CMessageHeader {
private:
    std::array<uint8_t, MESSAGE_START_SIZE> pchMessageStart{};
    char command[COMMAND_SIZE]{};
    uint32_t nMessageSize{0};
    uint32_t nChecksum{0};

public:
    CMessageHeader() = default;

    template<typename Stream>
    void Serialize(Stream& s) const {
        s.write(reinterpret_cast<const char*>(pchMessageStart.data()), pchMessageStart.size());
        s.write(command, COMMAND_SIZE);
        s << nMessageSize;
        s << nChecksum;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s.read(reinterpret_cast<char*>(pchMessageStart.data()), pchMessageStart.size());
        s.read(command, COMMAND_SIZE);
        s >> nMessageSize;
        s >> nChecksum;
    }

    bool IsValid(const Config& config) const;
    bool IsOversized(const Config& config) const;
};

class CNetMessage {
private:
    CMessageHeader hdr;
    CDataStream dataBuff;
    uint32_t nRecvVersion;
    mutable uint256 data_hash;
    CHashWriter hasher;

public:
    CNetMessage(const CMessageHeader& header, int nRecvVersionIn)
        : hdr(header), 
          dataBuff(nRecvVersionIn, SER_NETWORK, INIT_PROTO_VERSION),
          nRecvVersion(nRecvVersionIn),
          hasher(SER_NETWORK, INIT_PROTO_VERSION) {}

    bool complete() const { 
        return dataBuff.size() >= hdr.GetPayloadLength(); 
    }

    template<typename Stream>
    void Serialize(Stream& s) const {
        s << hdr;
        s << dataBuff;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> hdr;
        dataBuff.resize(hdr.GetMessageSize());
        s >> dataBuff;
    }

    const CMessageHeader& GetHeader() const { return hdr; }
    const uint256& GetMessageHash() const;
    uint64_t GetTotalLength() const;

    uint64_t Read(const Config& config, const char* pch, uint64_t nBytes);
    uint64_t Read(const Config& config, const uint8_t* pch, uint64_t nBytes);
};

#endif // BITCOIN_NET_NET_MESSAGE_H

