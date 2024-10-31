#include "test/test_bitcoin.h"
#include "net/ipv6_multicast.h"
#include "net/ipv6_multicast_processor.h"
#include "net/net.h"
#include "validation.h"
#include "chainparams.h"
#include "config.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(ipv6_multicast_tests, TestingSetup)

BOOST_AUTO_TEST_CASE(ipv6_multicast_message_serialization)
{
    // Test IPv6MulticastMessage serialization
    IPv6MulticastMessage msg;
    msg.type = MSG_TYPE_INV;
    msg.payload = std::vector<unsigned char>{0x01, 0x02, 0x03};

    CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
    ss << msg;

    IPv6MulticastMessage msg2;
    ss >> msg2;

    BOOST_CHECK_EQUAL(msg.type, msg2.type);
    BOOST_CHECK_EQUAL_COLLECTIONS(msg.payload.begin(), msg.payload.end(),
                                msg2.payload.begin(), msg2.payload.end());
}

BOOST_AUTO_TEST_CASE(ipv6_multicast_manager_basic)
{
    IPv6MulticastManager manager;
    
    // Test joining multicast group on loopback interface
    bool result = manager.JoinMulticastGroup("lo");
    BOOST_CHECK(result);

    // Test sending and receiving multicast message
    std::vector<unsigned char> testData = {0x01, 0x02, 0x03, 0x04};
    result = manager.SendMulticast(testData);
    BOOST_CHECK(result);

    std::vector<unsigned char> receivedData;
    result = manager.ReceiveMulticast(receivedData);
    BOOST_CHECK(result);
    BOOST_CHECK_EQUAL_COLLECTIONS(testData.begin(), testData.end(),
                                receivedData.begin(), receivedData.end());

    // Test leaving multicast group
    result = manager.LeaveMulticastGroup();
    BOOST_CHECK(result);
}

BOOST_AUTO_TEST_CASE(ipv6_multicast_processor_broadcast)
{
    auto& config = const_cast<Config&>(GetConfig());
    SelectParams(CBaseChainParams::REGTEST);

    // Create a CConnman instance
    CConnman connman{config, 0, true};
    
    // Create IPv6MulticastProcessor
    IPv6MulticastProcessor processor(connman);
    BOOST_CHECK(processor.Start());

    // Test broadcasting inventory
    CInv inv(MSG_TX, uint256S("0x1234567890abcdef"));
    bool result = processor.BroadcastInventory(inv);
    BOOST_CHECK(result);

    // Test broadcasting transaction
    CMutableTransaction tx;
    tx.nVersion = 1;
    result = processor.BroadcastTransaction(CTransaction(tx));
    BOOST_CHECK(result);

    // Test broadcasting block
    CBlock block;
    block.nVersion = 1;
    result = processor.BroadcastBlock(block);
    BOOST_CHECK(result);

    // Clean up
    BOOST_CHECK(processor.Stop());
}

BOOST_AUTO_TEST_CASE(ipv6_multicast_integration)
{
    auto& config = const_cast<Config&>(GetConfig());
    SelectParams(CBaseChainParams::REGTEST);

    // Create two CConnman instances to simulate two nodes
    CConnman connman1{config, 0, true};
    CConnman connman2{config, 0, true};

    // Enable IPv6 multicast on both nodes
    BOOST_CHECK(connman1.EnableIPv6Multicast());
    BOOST_CHECK(connman2.EnableIPv6Multicast());

    // Create and broadcast a transaction from node1
    CMutableTransaction tx;
    tx.nVersion = 1;
    CTransaction transaction(tx);
    
    BOOST_CHECK(connman1.BroadcastTransaction(transaction, true));

    // Wait briefly for multicast propagation
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Verify transaction was received by node2
    // This would require additional infrastructure to track received transactions
    // For now, we just verify the broadcast didn't fail

    // Clean up
    BOOST_CHECK(connman1.DisableIPv6Multicast());
    BOOST_CHECK(connman2.DisableIPv6Multicast());
}

BOOST_AUTO_TEST_CASE(ipv6_multicast_manager_initialization)
{
    IPv6MulticastManager manager;
    
    // Test joining multicast group on invalid interface
    bool result = manager.JoinMulticastGroup("invalid_interface");
    BOOST_CHECK(!result);

    // Test joining multicast group on loopback interface
    result = manager.JoinMulticastGroup("lo");
    BOOST_CHECK(result);

    // Test double join (should fail gracefully)
    result = manager.JoinMulticastGroup("lo");
    BOOST_CHECK(!result);

    // Test leaving multicast group
    result = manager.LeaveMulticastGroup();
    BOOST_CHECK(result);
}

BOOST_AUTO_TEST_CASE(ipv6_multicast_message_validation)
{
    // Test oversized message
    std::vector<unsigned char> largeData(1024 * 1024 + 1); // > 1MB
    IPv6MulticastManager manager;
    BOOST_CHECK(manager.JoinMulticastGroup("lo"));
    bool result = manager.SendMulticast(largeData);
    BOOST_CHECK(!result); // Should fail due to size

    // Test empty message
    std::vector<unsigned char> emptyData;
    result = manager.SendMulticast(emptyData);
    BOOST_CHECK(!result); // Should fail due to empty payload

    // Test valid message size
    std::vector<unsigned char> validData(1000);
    result = manager.SendMulticast(validData);
    BOOST_CHECK(result);
}

BOOST_AUTO_TEST_CASE(ipv6_multicast_processor_message_handling)
{
    auto& config = const_cast<Config&>(GetConfig());
    SelectParams(CBaseChainParams::REGTEST);

    CConnman connman{config, 0, true};
    IPv6MulticastProcessor processor(connman);
    
    // Test invalid transaction handling
    CMutableTransaction invalidTx;
    invalidTx.nVersion = 0; // Invalid version
    bool result = processor.BroadcastTransaction(CTransaction(invalidTx));
    BOOST_CHECK(!result);

    // Test valid transaction handling
    CMutableTransaction validTx;
    validTx.nVersion = 1;
    result = processor.BroadcastTransaction(CTransaction(validTx));
    BOOST_CHECK(result);

    // Test invalid block handling
    CBlock invalidBlock;
    invalidBlock.nVersion = 0; // Invalid version
    result = processor.BroadcastBlock(invalidBlock);
    BOOST_CHECK(!result);

    // Test valid block handling
    CBlock validBlock;
    validBlock.nVersion = 1;
    result = processor.BroadcastBlock(validBlock);
    BOOST_CHECK(result);
}

BOOST_AUTO_TEST_CASE(ipv6_multicast_network_conditions)
{
    auto& config = const_cast<Config&>(GetConfig());
    SelectParams(CBaseChainParams::REGTEST);

    // Test with multiple nodes
    std::vector<std::unique_ptr<CConnman>> nodes;
    const int NUM_NODES = 3;

    for (int i = 0; i < NUM_NODES; i++) {
        nodes.push_back(std::make_unique<CConnman>(config, 0, true));
        BOOST_CHECK(nodes.back()->EnableIPv6Multicast());
    }

    // Test broadcasting transaction across nodes
    CMutableTransaction tx;
    tx.nVersion = 1;
    CTransaction transaction(tx);
    
    // Broadcast from first node
    BOOST_CHECK(nodes[0]->BroadcastTransaction(transaction, true));

    // Clean up
    for (auto& node : nodes) {
        BOOST_CHECK(node->DisableIPv6Multicast());
    }
}

BOOST_AUTO_TEST_CASE(ipv6_multicast_stress_test)
{
    auto& config = const_cast<Config&>(GetConfig());
    SelectParams(CBaseChainParams::REGTEST);

    CConnman connman{config, 0, true};
    BOOST_CHECK(connman.EnableIPv6Multicast());

    // Rapid-fire message broadcasting
    const int NUM_MESSAGES = 100;
    for (int i = 0; i < NUM_MESSAGES; i++) {
        CMutableTransaction tx;
        tx.nVersion = 1;
        tx.vin.push_back(CTxIn()); // Add dummy input
        tx.vout.push_back(CTxOut()); // Add dummy output
        
        bool result = connman.BroadcastTransaction(CTransaction(tx), true);
        BOOST_CHECK(result);
    }

    // Allow time for processing
    std::this_thread::sleep_for(std::chrono::seconds(1));

    BOOST_CHECK(connman.DisableIPv6Multicast());
}

BOOST_AUTO_TEST_SUITE_END() 