#ifndef BITCOIN_NET_IPV6_MULTICAST_PROCESSOR_H
#define BITCOIN_NET_IPV6_MULTICAST_PROCESSOR_H

#include "ipv6_multicast.h"
#include "ipv6_multicast_message.h"
#include "net.h"
#include "protocol.h"
#include "sync.h"
#include <memory>
#include <thread>
#include <atomic>

class CConnman;

class IPv6MulticastProcessor {
public:
    IPv6MulticastProcessor(CConnman& connman);
    ~IPv6MulticastProcessor();

    bool Start();
    bool Stop();
    
    // Send various types of messages via multicast
    bool BroadcastInventory(const CInv& inv);
    bool BroadcastTransaction(const CTransaction& tx);
    bool BroadcastAddress(const CAddress& addr);
    bool BroadcastBlock(const CBlock& block);

private:
    CConnman& connman;
    std::unique_ptr<IPv6MulticastManager> multicastManager;
    std::thread receiveThread;
    std::atomic<bool> shouldStop{false};
    CCriticalSection cs_multicast;

    void ReceiveLoop();
    bool ProcessMessage(const IPv6MulticastMessage& msg);
    
    // Message handlers
    bool HandleInventory(const std::vector<unsigned char>& payload);
    bool HandleTransaction(const std::vector<unsigned char>& payload);
    bool HandleAddress(const std::vector<unsigned char>& payload);
    bool HandleBlock(const std::vector<unsigned char>& payload);

    // Utility functions
    template<typename T>
    bool SerializeForMulticast(const T& obj, std::vector<unsigned char>& data);
    
    template<typename T>
    bool DeserializeFromMulticast(const std::vector<unsigned char>& data, T& obj);
};

#endif // BITCOIN_NET_IPV6_MULTICAST_PROCESSOR_H 