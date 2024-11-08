// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2019 Bitcoin Association
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef BITCOIN_AMOUNT_H
#define BITCOIN_AMOUNT_H

#include "serialize.h"
#include <cstdint>

// Forward declarations
class CFeeRate;

/** Amount in satoshis (Can be negative) */
class Amount {
private:
    int64_t amount;

public:
    constexpr Amount() : amount(0) {}
    constexpr explicit Amount(int64_t _amount) : amount(_amount) {}

    static constexpr Amount zero() { return Amount(0); }
    static constexpr Amount satoshi() { return Amount(1); }

    int64_t GetSatoshis() const { return amount; }

    template<typename Stream>
    void Serialize(Stream& s) const {
        s << amount;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> amount;
    }

    friend bool operator<(const Amount& a, const Amount& b) {
        return a.amount < b.amount;
    }
    friend bool operator>(const Amount& a, const Amount& b) {
        return a.amount > b.amount;
    }
    friend bool operator==(const Amount& a, const Amount& b) {
        return a.amount == b.amount;
    }
    friend bool operator!=(const Amount& a, const Amount& b) {
        return !(a == b);
    }
    friend bool operator<=(const Amount& a, const Amount& b) {
        return !(a > b);
    }
    friend bool operator>=(const Amount& a, const Amount& b) {
        return !(a < b);
    }
    friend Amount operator+(const Amount& a, const Amount& b) {
        return Amount(a.amount + b.amount);
    }
};

/** Fee rate in satoshis per KB */
class CFeeRate {
private:
    Amount nSatoshisPerK{0};

public:
    CFeeRate() = default;
    explicit CFeeRate(const Amount& _nSatoshisPerK) : nSatoshisPerK(_nSatoshisPerK) {}

    Amount GetFee(size_t bytes) const;
    Amount GetFeePerK() const { return nSatoshisPerK; }

    template<typename Stream>
    void Serialize(Stream& s) const {
        s << nSatoshisPerK;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> nSatoshisPerK;
    }

    friend bool operator<(const CFeeRate& a, const CFeeRate& b) {
        return a.nSatoshisPerK < b.nSatoshisPerK;
    }
    friend bool operator>(const CFeeRate& a, const CFeeRate& b) {
        return a.nSatoshisPerK > b.nSatoshisPerK;
    }
    friend bool operator==(const CFeeRate& a, const CFeeRate& b) {
        return a.nSatoshisPerK == b.nSatoshisPerK;
    }
    friend bool operator<=(const CFeeRate& a, const CFeeRate& b) {
        return a.nSatoshisPerK <= b.nSatoshisPerK;
    }
    friend bool operator>=(const CFeeRate& a, const CFeeRate& b) {
        return a.nSatoshisPerK >= b.nSatoshisPerK;
    }
    friend bool operator!=(const CFeeRate& a, const CFeeRate& b) {
        return a.nSatoshisPerK != b.nSatoshisPerK;
    }
};

#endif // BITCOIN_AMOUNT_H
