#ifndef BITCOIN_NET_IPV6_MULTICAST_H
#define BITCOIN_NET_IPV6_MULTICAST_H

#include <netinet/in.h>
#include <string>
#include <vector>

class IPv6MulticastManager {
public:
    static const std::string DEFAULT_IPV6_MULTICAST_ADDR;  // ff02::1 - link-local all-nodes
    static const uint16_t DEFAULT_MULTICAST_PORT = 8333;

    IPv6MulticastManager();
    ~IPv6MulticastManager();

    bool JoinMulticastGroup(const std::string& interface_name);
    bool LeaveMulticastGroup();
    bool SendMulticast(const std::vector<unsigned char>& data);
    bool ReceiveMulticast(std::vector<unsigned char>& data);

private:
    int socket_fd;
    struct sockaddr_in6 multicast_addr;
    bool SetupSocket();
    bool SetMulticastOptions();
};

#endif // BITCOIN_NET_IPV6_MULTICAST_H 