// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2018-2019 Bitcoin Association
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef BITCOIN_SCRIPT_SCRIPT_H
#define BITCOIN_SCRIPT_SCRIPT_H

#include "consensus/consensus.h"
#include "crypto/common.h"
#include "prevector.h"
#include "serialize.h"
#include "opcodes.h"

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <cstring>
#include <limits>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

// Maximum number of bytes pushable to the stack -- replaced with DEFAULT_STACK_MEMORY_USAGE after Genesis
static const unsigned int MAX_SCRIPT_ELEMENT_SIZE_BEFORE_GENESIS = 520;

// Maximum number of elements on the stack -- replaced with DEFAULT_STACK_MEMORY_USAGE after Genesis
static const unsigned int MAX_STACK_ELEMENTS_BEFORE_GENESIS = 1000;

// Threshold for nLockTime: below this value it is interpreted as block number,
// otherwise as UNIX timestamp. Thresold is Tue Nov 5 00:53:20 1985 UTC
static const unsigned int LOCKTIME_THRESHOLD = 500000000;

template <typename T> std::vector<uint8_t> ToByteVector(const T &in) {
    return std::vector<uint8_t>(in.begin(), in.end());
}

class CScriptNum;

typedef prevector<28, uint8_t> CScriptBase;

namespace bsv
{
    class instruction_iterator;
}

/** Serialized script, used inside transaction inputs and outputs */
class CScript : public std::vector<uint8_t> {
public:
    using const_iterator = std::vector<uint8_t>::const_iterator;
    using iterator = std::vector<uint8_t>::iterator;

    CScript() = default;
    CScript(const_iterator pbegin, const_iterator pend) 
        : std::vector<uint8_t>(pbegin, pend) {}

    // Make these methods public since they're needed externally
    using std::vector<uint8_t>::clear;
    using std::vector<uint8_t>::begin;
    using std::vector<uint8_t>::end;
    using std::vector<uint8_t>::size;
    using std::vector<uint8_t>::empty;
    using std::vector<uint8_t>::data;
    using std::vector<uint8_t>::resize;

    bool IsUnspendable(bool isGenesisEnabled) const;

    template<typename Stream>
    void Serialize(Stream& s) const {
        WriteCompactSize(s, size());
        if (!empty()) {
            s.write(reinterpret_cast<const char*>(data()), size());
        }
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        size_t size = ReadCompactSize(s);
        resize(size);
        if (size > 0) {
            s.read(reinterpret_cast<char*>(data()), size);
        }
    }
};
static_assert(std::ranges::range<CScript>);

std::ostream &operator<<(std::ostream &, const CScript &);
std::string to_string(const CScript&);

bool IsP2SH(std::span<const uint8_t>);
bool IsDSNotification(std::span<const uint8_t>);
bool IsDustReturnScript(std::span<const uint8_t> script);
bool IsMinerId(std::span<const uint8_t> script);

constexpr bool IsMinerInfo(const std::span<const uint8_t> script)
{
    constexpr std::array<uint8_t, 4> protocol_id{0x60, 0x1d, 0xfa, 0xce};
    return script.size() >= 7 && 
           script[0] == OP_FALSE &&
           script[1] == OP_RETURN && 
           script[2] == protocol_id.size() &&
           script[3] == protocol_id[0] &&
           script[4] == protocol_id[1] &&
           script[5] == protocol_id[2] &&
           script[6] == protocol_id[3];
}

size_t CountOp(std::span<const uint8_t>, opcodetype);

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class CReserveScript {
public:
    CScript reserveScript;
    virtual void KeepScript() {}
    CReserveScript() {}
    virtual ~CReserveScript() {}
};

#endif // BITCOIN_SCRIPT_SCRIPT_H
