#include "ipv6_multicast_processor.h"
#include "net.h"
#include "validation.h"
#include "util.h"
#include "streams.h"
#include "primitives/block.h"
#include "primitives/transaction.h"
#include <boost/algorithm/string.hpp>

IPv6MulticastProcessor::IPv6MulticastProcessor(CConnman& conn) 
    : connman(conn), multicastManager(std::make_unique<IPv6MulticastManager>()) {}

IPv6MulticastProcessor::~IPv6MulticastProcessor() {
    Stop();
}

bool IPv6MulticastProcessor::Start() {
    // Try to join multicast group on all available interfaces
    bool joined = false;
    std::vector<std::string> interfaces = GetNetworkInterfaces();
    for (const auto& interface : interfaces) {
        if (multicastManager->JoinMulticastGroup(interface)) {
            joined = true;
            LogPrintf("Joined IPv6 multicast group on interface %s\n", interface);
        }
    }

    if (!joined) {
        LogPrintf("Failed to join IPv6 multicast group on any interface\n");
        return false;
    }

    // Start receive thread
    shouldStop = false;
    receiveThread = std::thread(&IPv6MulticastProcessor::ReceiveLoop, this);
    return true;
}

bool IPv6MulticastProcessor::Stop() {
    shouldStop = true;
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
    return multicastManager->LeaveMulticastGroup();
}

void IPv6MulticastProcessor::ReceiveLoop() {
    std::vector<unsigned char> data;
    while (!shouldStop) {
        if (multicastManager->ReceiveMulticast(data)) {
            try {
                CDataStream stream(data, SER_NETWORK, PROTOCOL_VERSION);
                IPv6MulticastMessage msg;
                stream >> msg;
                ProcessMessage(msg);
            } catch (const std::exception& e) {
                LogPrintf("Error processing IPv6 multicast message: %s\n", e.what());
            }
        }
    }
}

bool IPv6MulticastProcessor::ProcessMessage(const IPv6MulticastMessage& msg) {
    switch (msg.type) {
        case MSG_TYPE_INV:
            return HandleInventory(msg.payload);
        case MSG_TYPE_TX:
            return HandleTransaction(msg.payload);
        case MSG_TYPE_ADDR:
            return HandleAddress(msg.payload);
        case MSG_TYPE_BLOCK:
            return HandleBlock(msg.payload);
        default:
            LogPrintf("Unknown IPv6 multicast message type: %d\n", msg.type);
            return false;
    }
}

bool IPv6MulticastProcessor::HandleInventory(const std::vector<unsigned char>& payload) {
    LOCK(cs_multicast);
    try {
        std::vector<CInv> vInv;
        CDataStream stream(payload, SER_NETWORK, PROTOCOL_VERSION);
        stream >> vInv;

        // Process each inventory item
        for (const CInv& inv : vInv) {
            // Forward to regular P2P network processing
            connman.ForEachNode([&inv](const CNodePtr& pnode) {
                if (pnode->fSuccessfullyConnected) {
                    pnode->PushInventory(inv);
                }
            });
        }
        return true;
    } catch (const std::exception& e) {
        LogPrintf("Error handling multicast inventory: %s\n", e.what());
        return false;
    }
}

bool IPv6MulticastProcessor::HandleTransaction(const std::vector<unsigned char>& payload) {
    LOCK(cs_multicast);
    try {
        CTransaction tx;
        CDataStream stream(payload, SER_NETWORK, PROTOCOL_VERSION);
        stream >> tx;

        // Basic transaction validation
        if (!tx.IsCoinBase() && !tx.HasValidFee()) {
            LogPrintf("Rejecting invalid multicast transaction\n");
            return false;
        }

        // Create inventory message for this transaction
        CInv inv(MSG_TX, tx.GetHash());
        
        // Forward to regular P2P network processing
        connman.ForEachNode([&inv, &tx](const CNodePtr& pnode) {
            if (pnode->fSuccessfullyConnected) {
                pnode->PushInventory(inv);
                // Also send the actual transaction
                pnode->PushMessage("tx", tx);
            }
        });
        return true;
    } catch (const std::exception& e) {
        LogPrintf("Error handling multicast transaction: %s\n", e.what());
        return false;
    }
}

bool IPv6MulticastProcessor::HandleAddress(const std::vector<unsigned char>& payload) {
    LOCK(cs_multicast);
    try {
        std::vector<CAddress> vAddr;
        CDataStream stream(payload, SER_NETWORK, PROTOCOL_VERSION);
        stream >> vAddr;

        // Basic address validation and processing
        std::vector<CAddress> vAddrOk;
        const int64_t nNow = GetAdjustedTime();
        const int64_t nSince = nNow - 10 * 60;

        for (CAddress& addr : vAddr) {
            // Basic timestamp sanity check
            if (addr.nTime <= 100000000 || addr.nTime > nNow + 10 * 60) {
                addr.nTime = nNow - 5 * 24 * 60 * 60;
            }

            // Check for required services and reachability
            if ((addr.nServices & REQUIRED_SERVICES) == REQUIRED_SERVICES && 
                IsReachable(addr) && 
                addr.nTime > nSince && 
                addr.IsRoutable()) {
                vAddrOk.push_back(addr);
            }
        }

        // Add valid addresses to address manager
        if (!vAddrOk.empty()) {
            connman.AddNewAddresses(vAddrOk, CService(), 2 * 60 * 60);
        }
        return true;
    } catch (const std::exception& e) {
        LogPrintf("Error handling multicast address: %s\n", e.what());
        return false;
    }
}

bool IPv6MulticastProcessor::HandleBlock(const std::vector<unsigned char>& payload) {
    LOCK(cs_multicast);
    try {
        CBlock block;
        CDataStream stream(payload, SER_NETWORK, PROTOCOL_VERSION);
        stream >> block;

        // Create inventory message for this block
        CInv inv(MSG_BLOCK, block.GetHash());

        // Forward to regular P2P network processing
        connman.ForEachNode([&inv, &block](const CNodePtr& pnode) {
            if (pnode->fSuccessfullyConnected) {
                pnode->PushInventory(inv);
                // Optionally send the full block if node requests it
                // This will happen through normal P2P getdata mechanism
            }
        });
        return true;
    } catch (const std::exception& e) {
        LogPrintf("Error handling multicast block: %s\n", e.what());
        return false;
    }
}

bool IPv6MulticastProcessor::BroadcastInventory(const CInv& inv) {
    LOCK(cs_multicast);
    try {
        std::vector<unsigned char> data;
        std::vector<CInv> vInv = {inv};
        if (!SerializeForMulticast(vInv, data)) {
            return false;
        }

        IPv6MulticastMessage msg;
        msg.type = MSG_TYPE_INV;
        msg.payload = data;

        CDataStream stream(SER_NETWORK, PROTOCOL_VERSION);
        stream << msg;
        return multicastManager->SendMulticast(std::vector<unsigned char>(stream.begin(), stream.end()));
    } catch (const std::exception& e) {
        LogPrintf("Error broadcasting inventory: %s\n", e.what());
        return false;
    }
}

template<typename T>
bool IPv6MulticastProcessor::SerializeForMulticast(const T& obj, std::vector<unsigned char>& data) {
    try {
        CDataStream stream(SER_NETWORK, PROTOCOL_VERSION);
        stream << obj;
        data.assign(stream.begin(), stream.end());
        return true;
    } catch (const std::exception& e) {
        LogPrintf("Error serializing for multicast: %s\n", e.what());
        return false;
    }
}

template<typename T>
bool IPv6MulticastProcessor::DeserializeFromMulticast(const std::vector<unsigned char>& data, T& obj) {
    try {
        CDataStream stream(data, SER_NETWORK, PROTOCOL_VERSION);
        stream >> obj;
        return true;
    } catch (const std::exception& e) {
        LogPrintf("Error deserializing from multicast: %s\n", e.what());
        return false;
    }
} 