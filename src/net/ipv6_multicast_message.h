#ifndef BITCOIN_NET_IPV6_MULTICAST_MESSAGE_H
#define BITCOIN_NET_IPV6_MULTICAST_MESSAGE_H

#include "serialize.h"
#include <string>
#include <vector>

// Message types for IPv6 multicast
enum IPv6MulticastMessageType : uint8_t {
    MSG_TYPE_INV = 1,
    MSG_TYPE_TX = 2,
    MSG_TYPE_ADDR = 3,
    MSG_TYPE_BLOCK = 4
};

class IPv6MulticastMessage {
public:
    IPv6MulticastMessageType type;
    std::vector<unsigned char> payload;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(type);
        READWRITE(payload);
    }
};

#endif // BITCOIN_NET_IPV6_MULTICAST_MESSAGE_H 