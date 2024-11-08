#include "ipv6_multicast.h"
#include "util.h"
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

const std::string IPv6MulticastManager::DEFAULT_IPV6_MULTICAST_ADDR = "ff02::1";

IPv6MulticastManager::IPv6MulticastManager() : socket_fd(-1) {
    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin6_family = AF_INET6;
    multicast_addr.sin6_port = htons(DEFAULT_MULTICAST_PORT);
    inet_pton(AF_INET6, DEFAULT_IPV6_MULTICAST_ADDR.c_str(), &multicast_addr.sin6_addr);
}

IPv6MulticastManager::~IPv6MulticastManager() {
    if (socket_fd >= 0) {
        LeaveMulticastGroup();
        close(socket_fd);
    }
}

bool IPv6MulticastManager::SetupSocket() {
    socket_fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        LogPrintf("Error creating IPv6 multicast socket\n");
        return false;
    }

    // Enable SO_REUSEADDR to allow multiple instances to run simultaneously
    int reuse = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        LogPrintf("Error setting SO_REUSEADDR on IPv6 multicast socket\n");
        return false;
    }

    // Bind to the multicast port
    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(DEFAULT_MULTICAST_PORT);
    addr.sin6_addr = in6addr_any;

    if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        LogPrintf("Error binding IPv6 multicast socket\n");
        return false;
    }

    return SetMulticastOptions();
}

bool IPv6MulticastManager::SetMulticastOptions() {
    // Set hop limit for multicast packets
    int hops = 1;  // Restrict to local network
    if (setsockopt(socket_fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hops, sizeof(hops)) < 0) {
        LogPrintf("Error setting IPv6 multicast hop limit\n");
        return false;
    }

    // Disable loopback
    int loopback = 0;
    if (setsockopt(socket_fd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &loopback, sizeof(loopback)) < 0) {
        LogPrintf("Error disabling IPv6 multicast loopback\n");
        return false;
    }

    return true;
}

bool IPv6MulticastManager::JoinMulticastGroup(const std::string& interface_name) {
    if (!SetupSocket()) return false;
    
    struct ipv6_mreq mreq;
    memcpy(&mreq.ipv6mr_multiaddr, &multicast_addr.sin6_addr, sizeof(struct in6_addr));
    mreq.ipv6mr_interface = if_nametoindex(interface_name.c_str());
    
    if (setsockopt(socket_fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq)) < 0) {
        LogPrintf("Error joining IPv6 multicast group\n");
        return false;
    }
    return true;
}

bool IPv6MulticastManager::LeaveMulticastGroup() {
    if (socket_fd < 0) return false;

    struct ipv6_mreq mreq;
    memcpy(&mreq.ipv6mr_multiaddr, &multicast_addr.sin6_addr, sizeof(struct in6_addr));
    mreq.ipv6mr_interface = 0;  // Leave on all interfaces

    if (setsockopt(socket_fd, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &mreq, sizeof(mreq)) < 0) {
        LogPrintf("Error leaving IPv6 multicast group\n");
        return false;
    }
    return true;
}

bool IPv6MulticastManager::SendMulticast(const std::vector<unsigned char>& data) {
    if (socket_fd < 0) return false;

    ssize_t sent = sendto(socket_fd, data.data(), data.size(), 0,
                         (struct sockaddr*)&multicast_addr, sizeof(multicast_addr));
    
    if (sent < 0 || static_cast<size_t>(sent) != data.size()) {
        LogPrintf("Error sending IPv6 multicast message\n");
        return false;
    }
    return true;
}

bool IPv6MulticastManager::ReceiveMulticast(std::vector<unsigned char>& data) {
    if (socket_fd < 0) return false;

    unsigned char buffer[65536];  // Maximum UDP packet size
    struct sockaddr_in6 sender_addr;
    socklen_t sender_addr_len = sizeof(sender_addr);

    ssize_t received = recvfrom(socket_fd, buffer, sizeof(buffer), 0,
                               (struct sockaddr*)&sender_addr, &sender_addr_len);

    if (received < 0) {
        LogPrintf("Error receiving IPv6 multicast message\n");
        return false;
    }

    data.assign(buffer, buffer + received);
    return true;
}