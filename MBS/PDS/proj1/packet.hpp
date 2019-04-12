#ifndef packet_hpp
#define packet_hpp

#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <climits>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <vector>
#include <ifaddrs.h>
#include <iostream>
#include <mutex>
#include <chrono>

#define CHADDR_LENGTH 16
#define SNAME_LENGTH 64
#define FILE_LENGTH 128

#define DHCP_DISCOVER 1
#define DHCP_OFFER 2
#define DHCP_REQUEST 3
#define DHCP_ACK 5

#define DHCP_BOOTREQUEST 1
#define DHCP_BOOTREPLY 2

#define DHCP_HARDWARE_TYPE_10_EHTHERNET 1
#define DHCP_OFFER_TIMEOUT 120000

#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68
#define MESSAGE_TYPE_REQ_IP 50
#define MESSAGE_TYPE_SERVER_IP 54
#define MESSAGE_TYPE_CLIENT_ID 61
#define MESSAGE_TYPE_HOSTNAME 12
#define MESSAGE_TYPE_ROUTER 3
#define MESSAGE_TYPE_DNS 6
#define MESSAGE_TYPE_MASK 1
#define MESSAGE_TYPE_DOMAIN_NAME 15
#define MESSAGE_TYPE_LEASE_TIME 51

#define DHCP_MAGIC_COOKIE 0x63825363
#define MESSAGE_TYPE_DHCP 53
#define MESSAGE_TYPE_END 255

typedef u_int32_t ipv4_addr;

//structure that bears parsed arguments as a server
typedef struct params {
    u_int32_t startingIp;
    u_int32_t endingIp;
    u_int32_t dnsServer;
    u_int32_t gateway;
    u_int32_t mask;
    std::string interface;
    char* domain;
    u_int32_t leaseTime;
} serverParams;

//structure that keeps information about one IP address from given pool
typedef struct clients {
    ipv4_addr ip;
    u_int8_t mac[CHADDR_LENGTH];
    bool isOffered;
    bool isAcked;
    std::chrono::time_point<std::chrono::system_clock> offeredTime;
} clientsList;

//dhcp packet structure (pragma used for avoiding packet padding)
//dhcp header options solution - inspired from https://github.com/samueldotj/dhcp-client
#pragma pack(push, 1)
typedef struct dhcp
{
    u_int8_t op;
    u_int8_t htype;
    u_int8_t hlen;
    u_int8_t hops;
    u_int32_t xid;
    u_int16_t secs;
    u_int16_t flags;
    ipv4_addr ciaddr;
    ipv4_addr yiaddr;
    ipv4_addr siaddr;
    ipv4_addr giaddr;
    u_int8_t chaddr[CHADDR_LENGTH];
    char bp_sname[SNAME_LENGTH];
    char bp_file[FILE_LENGTH];
    uint32_t magic_cookie;
    u_int8_t bp_options[0];
} dhcp_packet;
#pragma pack(pop)

//structure that keeps all the important DHCP information from each request/response so new request/response can be created
typedef struct protocol
{
    u_int8_t mac[CHADDR_LENGTH];
    ipv4_addr proposedIp;
    ipv4_addr serverIp;
    u_int8_t messageType;
    u_int32_t xid;
} protocolIp;

class Packet {
private:
	struct udphdr *udp_header;
    struct ip *ip_header;
    dhcp_packet *dhcp;
    struct ether_header *eframe;
	u_int32_t ip;
    char packet[4096];
    int len;
    serverParams params;
    u_int8_t serverMac[CHADDR_LENGTH];
    
public:
	Packet(pcap_t* pcap);
    void clientListener();
    void serverListener(serverParams params);
	void createPacket(u_int8_t option, protocolIp data);
	int fillOption(u_int8_t *packet, u_int8_t code, u_int8_t *data, u_int8_t len);
	unsigned short in_cksum(unsigned short *addr, int len);
    void sendPacket();
    pcap_t*pcap_handle;
    std::vector<protocolIp> protocol;
    std::mutex m;
};

#endif
