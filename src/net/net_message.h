// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2017 The Bitcoin developers
// Copyright (c) 2020-2021 Bitcoin Association
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#pragma once

#include <hash.h>
#include <protocol.h>
#include <streams.h>
#include "msg_buffer.h"

#include <stdexcept>
#include <string>
#include <vector>

#ifndef BITCOIN_NET_NET_MESSAGE_H
#define BITCOIN_NET_NET_MESSAGE_H

#include "serialize.h"

class CNetMessage {
public:
    using PayloadType = std::vector<uint8_t>;
    
    CNetMessage() = default;
    explicit CNetMessage(PayloadType&& payload) : data(std::move(payload)) {}

    const PayloadType& GetData() const { return data; }
    PayloadType& GetData() { return data; }

    template<typename Stream>
    void Serialize(Stream& s) const {
        ::Serialize(s, data);
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        ::Unserialize(s, data);
    }

private:
    PayloadType data;
};

#endif // BITCOIN_NET_NET_MESSAGE_H

