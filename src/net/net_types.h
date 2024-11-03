// Copyright (c) 2020 Bitcoin Association
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

class CNode;
using NodeId = int64_t;
using CNodePtr = std::shared_ptr<CNode>;

enum class StreamType : uint8_t {
    UNKNOWN,
    GENERAL,
    BLOCK,
    TRANSACTION,
    NUM_STREAM_TYPES
};

template<typename Stream>
void Serialize(Stream& s, const StreamType& st) {
    s << static_cast<uint8_t>(st);
}

template<typename Stream>
void Unserialize(Stream& s, StreamType& st) {
    uint8_t val;
    s >> val;
    st = static_cast<StreamType>(val);
}

struct StreamStats {
    StreamType streamType;
    int64_t nLastSend{0};
    int64_t nLastRecv{0};
    uint64_t nSendBytes{0};
    uint64_t nRecvBytes{0};
    uint64_t nSendSize{0};
    uint64_t nRecvSize{0};
    std::map<std::string, uint64_t> mapSendBytesPerMsgCmd;
    std::map<std::string, uint64_t> mapRecvBytesPerMsgCmd;

    template<typename Stream>
    void Serialize(Stream& s) const {
        s << streamType;
        s << nLastSend;
        s << nLastRecv;
        s << nSendBytes;
        s << nRecvBytes;
        s << nSendSize;
        s << nRecvSize;
        s << mapSendBytesPerMsgCmd;
        s << mapRecvBytesPerMsgCmd;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> streamType;
        s >> nLastSend;
        s >> nLastRecv;
        s >> nSendBytes;
        s >> nRecvBytes;
        s >> nSendSize;
        s >> nRecvSize;
        s >> mapSendBytesPerMsgCmd;
        s >> mapRecvBytesPerMsgCmd;
    }
};

/** Some common type alises used throughout the networking code */

// A node ID
using NodeId = int64_t;

// Command, total bytes
using mapMsgCmdSize = std::map<std::string, uint64_t>;

// Average bandwidth, number of items average is calculated over
using AverageBandwidth = std::pair<uint64_t, size_t>;

/** Some constants */

// Peer average bandwidth measurement interval
static const unsigned PEER_AVG_BANDWIDTH_CALC_FREQUENCY_SECS = 5;

// Reject codes for stream errors
static const uint8_t REJECT_STREAM_SETUP = 0x60;

// Maximum length for a serialised StreamPolicy name
static const size_t MAX_STREAM_POLICY_NAME_LENGTH = 64;
