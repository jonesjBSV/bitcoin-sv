// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UINT256_H
#define BITCOIN_UINT256_H

#include "crypto/common.h"
#include "utilstrencodings.h"
#include "serialize.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include "boost/functional/hash.hpp"

template <unsigned int BITS>
class base_blob {
protected:
    enum { WIDTH = BITS / 8 };
    uint8_t data[WIDTH];

public:
    base_blob() { memset(data, 0, sizeof(data)); }
    
    template<typename T>
    base_blob(T first, T last) {
        assert(std::distance(first, last) == sizeof(data));
        std::copy(first, last, &data[0]);
    }

    explicit base_blob(const std::vector<uint8_t>& vch) {
        assert(vch.size() == sizeof(data));
        memcpy(data, &vch[0], sizeof(data));
    }

    template<typename Stream>
    void Serialize(Stream& s) const {
        s.write(reinterpret_cast<const char*>(data), sizeof(data));
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s.read(reinterpret_cast<char*>(data), sizeof(data));
    }

    bool IsNull() const {
        for (int i = 0; i < WIDTH; i++)
            if (data[i] != 0) return false;
        return true;
    }

    void SetNull() { memset(data, 0, sizeof(data)); }

    inline int Compare(const base_blob& other) const {
        return memcmp(data, other.data, sizeof(data));
    }

    friend inline bool operator==(const base_blob& a, const base_blob& b) {
        return a.Compare(b) == 0;
    }
    friend inline bool operator!=(const base_blob& a, const base_blob& b) {
        return a.Compare(b) != 0;
    }
    friend inline bool operator<(const base_blob& a, const base_blob& b) {
        return a.Compare(b) < 0;
    }

    std::string GetHex() const;
    void SetHex(const char* psz);
    void SetHex(const std::string& str);
    std::string ToString() const;

    uint64_t GetCheapHash() const {
        return ReadLE64(data);
    }
};

// Specializations for uint256 and uint160
using uint256 = base_blob<256>;
using uint160 = base_blob<160>;

namespace std {
    template<>
    struct hash<uint256> {
        size_t operator()(const uint256& u) const {
            return static_cast<size_t>(u.GetCheapHash());
        }
    };
}

inline uint256 uint256S(const char* str) {
    uint256 rv;
    rv.SetHex(str);
    return rv;
}

inline uint256 uint256S(const std::string& str) {
    uint256 rv;
    rv.SetHex(str);
    return rv;
}

#endif // BITCOIN_UINT256_H
