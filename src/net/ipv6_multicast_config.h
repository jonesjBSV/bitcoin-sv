#ifndef BITCOIN_NET_IPV6_MULTICAST_CONFIG_H
#define BITCOIN_NET_IPV6_MULTICAST_CONFIG_H

// Default values for IPv6 multicast configuration
static const bool DEFAULT_MULTICAST = false;
static const bool DEFAULT_IPV6 = true;
static const bool DEFAULT_MULTICAST_LOOP = true;
static const int DEFAULT_MULTICAST_TTL = 1;
static const int DEFAULT_P2P_PORT = 8333;

// Maximum values for validation
static const int MAX_MULTICAST_TTL = 255;
static const int MIN_MULTICAST_TTL = 0;

#endif // BITCOIN_NET_IPV6_MULTICAST_CONFIG_H 