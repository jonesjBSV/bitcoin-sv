#ifndef BITCOIN_NET_SERIALIZED_NET_MSG_H
#define BITCOIN_NET_SERIALIZED_NET_MSG_H

#include <vector>
#include <cstdint>
#include <string>
#include <memory>
#include "serialize.h"
#include "streams.h"

class CSerializedNetMsg {
public:
    using PayloadType = std::vector<uint8_t>;
    
    enum class Type : uint8_t {
        UNKNOWN = 0,
        BLOCK = 1,
        TX = 2,
        FILTERED_BLOCK = 3,
        CMPCT_BLOCK = 4
    };
    
    CSerializedNetMsg() = default;
    CSerializedNetMsg(CSerializedNetMsg&&) = default;
    CSerializedNetMsg& operator=(CSerializedNetMsg&&) = default;

    CSerializedNetMsg(std::string&& cmd, Type type, PayloadType&& payload)
        : command(std::move(cmd)), payloadType(type), data(std::move(payload)) {}

    explicit CSerializedNetMsg(PayloadType&& payload)
        : data(std::move(payload))
    {}

    const std::string& Command() const { return command; }
    Type GetPayloadType() const { return payloadType; }
    const PayloadType& GetData() const { return data; }
    PayloadType&& MoveData() { return std::move(data); }
    size_t Size() const { return data.size(); }

    template<typename Stream>
    void Serialize(Stream& s) const {
        s << command;
        uint8_t type = static_cast<uint8_t>(payloadType);
        s << type;
        s << data;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> command;
        uint8_t type;
        s >> type;
        payloadType = static_cast<Type>(type);
        s >> data;
    }

    size_t GetEstimatedMemoryUsage() const {
        return command.capacity() + data.capacity();
    }

    bool IsValid() const {
        return !command.empty() && !data.empty();
    }

    std::string ToString() const {
        return strprintf("CSerializedNetMsg(command=%s, type=%d, size=%u)",
                        command, static_cast<int>(payloadType), data.size());
    }

private:
    std::string command;
    Type payloadType{Type::UNKNOWN};
    PayloadType data;
};

#endif // BITCOIN_NET_SERIALIZED_NET_MSG_H 