// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_NETMESSAGEMAKER_H
#define BITCOIN_NETMESSAGEMAKER_H

#include "net/net.h"
#include "serialize.h"
#include "net/serialized_net_msg.h"

class CNetMsgMaker {
public:
    explicit CNetMsgMaker(int nVersionIn) : nVersion(nVersionIn) {}

    template <typename... Args>
    CSerializedNetMsg Make(int nFlags, CSerializedNetMsg::Type payloadType, 
                          std::string sCommand, Args&&... args) const {
        CSerializedNetMsg::PayloadType data;
        size_t nSize = GetSerializeSize(std::forward<Args>(args)..., nVersion | nFlags);
        data.reserve(nSize);
        CVectorWriter{SER_NETWORK, nFlags | nVersion, data, 0,
                     std::forward<Args>(args)...};
        return {std::move(sCommand), payloadType, std::move(data)};
    }

    template <typename... Args>
    CSerializedNetMsg Make(std::string sCommand, Args&&... args) const {
        return Make(0, CSerializedNetMsg::Type::UNKNOWN, std::move(sCommand),
                   std::forward<Args>(args)...);
    }

    int GetVersion() const { return nVersion; }

private:
    const int nVersion;
};

#endif // BITCOIN_NETMESSAGEMAKER_H
