#ifndef BITCOIN_NET_NET_EXCEPTIONS_H
#define BITCOIN_NET_NET_EXCEPTIONS_H

#include <stdexcept>
#include <string>
#include "serialize.h"

// Exception thrown when peer should be banned
class BanPeer : public std::runtime_error {
private:
    std::string reason;
    int banTime {0}; // Duration of ban in seconds

public:
    explicit BanPeer(const std::string& msg, int banDuration = 24*60*60) 
        : std::runtime_error(msg)
        , reason(msg)
        , banTime(banDuration)
    {}

    const std::string& GetReason() const { return reason; }
    int GetBanTime() const { return banTime; }

    template<typename Stream>
    void Serialize(Stream& s) const {
        s << reason;
        s << banTime;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> reason;
        s >> banTime;
    }
};

// Exception thrown when network message is invalid
class InvalidNetMsg : public std::runtime_error {
private:
    std::string command;
    std::string details;

public:
    InvalidNetMsg(const std::string& cmd, const std::string& msg) 
        : std::runtime_error(msg)
        , command(cmd)
        , details(msg)
    {}

    const std::string& GetCommand() const { return command; }
    const std::string& GetDetails() const { return details; }

    template<typename Stream>
    void Serialize(Stream& s) const {
        s << command;
        s << details;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> command;
        s >> details;
    }
};

// Exception thrown when message content is invalid
class InvalidMessage : public std::runtime_error {
private:
    std::string msgType;

public:
    InvalidMessage(const std::string& type, const std::string& msg) 
        : std::runtime_error(msg)
        , msgType(type)
    {}

    const std::string& GetMessageType() const { return msgType; }

    template<typename Stream>
    void Serialize(Stream& s) const {
        s << msgType;
        s << std::string(what());
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> msgType;
        std::string msg;
        s >> msg;
        // Note: what() is const char*, we can't modify it directly
    }
};

#endif // BITCOIN_NET_NET_EXCEPTIONS_H 