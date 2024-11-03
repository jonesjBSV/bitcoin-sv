// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2017 The Bitcoin developers
// Copyright (c) 2020-2021 Bitcoin Association
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#include <net/net_message.h>
#include <logging.h>

uint64_t CNetMessage::Read(const Config& config, const char* pch, uint64_t nBytes)
{
    // Still reading header?
    if(!hdr.Complete())
    {
        try
        {
            uint64_t numRead { hdr.Read(pch, nBytes, dataBuff) };
            if(hdr.Complete())
            {
                // Reject oversized messages
                if(hdr.IsOversized(config))
                {
                    throw std::runtime_error("Oversized header detected");
                }

                dataBuff.resize(hdr.GetMessageSize());
            }

            return numRead;
        }
        catch(const std::exception& e)
        {
            throw std::runtime_error(std::string("Bad header format: ") + e.what());
        }
    }

    // Reading payload
    uint64_t nCopy = std::min(nBytes, hdr.GetPayloadLength() - dataBuff.size());
    if(nCopy > 0) {
        dataBuff.write(pch, nCopy);
        hasher.write(pch, nCopy);
    }

    return nCopy;
}

uint64_t CNetMessage::Read(const Config& config, const uint8_t* p, uint64_t nBytes)
{
    return Read(config, reinterpret_cast<const char*>(p), nBytes);
}

const uint256& CNetMessage::GetMessageHash() const
{
    if(data_hash.IsNull()) {
        // Create a non-const copy of hasher to call GetHash()
        CHashWriter tempHasher = hasher;
        data_hash = tempHasher.GetHash();
    }
    return data_hash;
}

uint64_t CNetMessage::GetTotalLength() const
{
    return hdr.GetLength();
}

