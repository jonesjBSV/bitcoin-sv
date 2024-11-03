#ifndef BITCOIN_NET_NET_EXCEPTIONS_H
#define BITCOIN_NET_NET_EXCEPTIONS_H

#include <stdexcept>
#include <string>

// Exception thrown when peer should be banned
class BanPeer : public std::runtime_error {
private:
    std::string reason;

public:
    explicit BanPeer(const std::string& msg) 
        : std::runtime_error(msg)
        , reason(msg) 
    {}

    const std::string& GetReason() const { return reason; }

    template<typename Stream>
    void Serialize(Stream& s) const {
        s << reason;
    }

    template<typename Stream>
    void Unserialize(Stream& s) {
        s >> reason;
    }
};

class InvalidNetMsg : public std::runtime_error {
public:
    explicit InvalidNetMsg(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidMessage : public std::runtime_error {
public:
    explicit InvalidMessage(const std::string& msg) : std::runtime_error(msg) {}
};

#endif // BITCOIN_NET_NET_EXCEPTIONS_H 