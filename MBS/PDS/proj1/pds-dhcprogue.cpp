/**
  * pds-dhcprogue
  * author - Jan Pawlus (xpawlu00@stud.fit.vutbr.cz)
**/

#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "packet.hpp"
#include <sys/mman.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <signal.h>
#include <utility>
#include <bitset>
#include <arpa/inet.h>
#include <errno.h>
#include <algorithm>

//global variable for pcap handle so it can be closed during SIGINT
pcap_t* pcap_handle;

void siginthandler(int param)
{
	pcap_close(pcap_handle);
	exit(0);
}

/* function that checks the ip pool and removes expired assigned IPs */
void refreshPool(std::vector<clientsList>* clients, u_int32_t leaseTime) {
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	for (auto i = clients->begin(); i != clients->end(); ++i)
	{	
		// if ip was offered but not acked
		if (i->isOffered && !(i->isAcked))
		{
			auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - i->offeredTime).count();
			if (milliseconds > DHCP_OFFER_TIMEOUT){
				i->isOffered = i->isAcked = false;
			}
		}
		// if ip was offered and acked and lease time os over
		else if (i->isOffered && i->isAcked)
		{
			auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - i->offeredTime).count();
			if (milliseconds > (leaseTime*1000)) {
				i->isOffered = i->isAcked = false;
			}
		}
	}
}

/* computes subnet mask according to given IP pool */
u_int32_t computeMask(u_int32_t startIp, u_int32_t endIp) {
	std::bitset<32> startBits(ntohl(startIp));
	std::bitset<32> endBits(ntohl(endIp));
	u_int32_t mask;
	mask = 0xFFFFFFFF;
	int i = 31;
	for (i = 31; i >= 0; i--) 
	{
		if (startBits[i] != endBits[i])
		{
			break;
		}
	}
	mask = mask << (i+1);
	return htonl(mask);
}

/* arg parser */
int parseArgs(int argc, char *argv[], serverParams* params)
{
	if (params == NULL)
		return -1;
	params->gateway = params->startingIp = params->endingIp = params->dnsServer = params->leaseTime = 0;
	params->domain = NULL;
	params->interface = std::string("");
	int c;
	char* poolArray;
	bool startIp = true;
	errno = 0;
	std::string start, end;
	char* endptr = NULL;
	while ((c = getopt(argc, argv, "i:p:g:n:l:d:")) != -1) {
		switch (c) {
			case 'i':
				params->interface = std::string(optarg);
				break;
			case 'p':
				poolArray = strtok(optarg, "-");
				
				while (poolArray != NULL)
				{
					struct in_addr addr;
					if (startIp)
					{
						if (!inet_pton(AF_INET, poolArray, &(params->startingIp)))
						{
							std::cerr << "Ip pool not in correct format\n";
							return -1;
						}
						start = std::string(poolArray);
						startIp = false;
					}
					else
					{
						if (!inet_pton(AF_INET, poolArray, &(params->endingIp)))
						{
							std::cerr << "Ip pool not in correct format.\n";
							return -1;
						}
						end = std::string(poolArray);
					}					
					poolArray = strtok (NULL, "-");
				}
				params->mask = computeMask(params->startingIp, params->endingIp);			
				break;
			case 'g':
				if (!inet_pton(AF_INET, optarg, &(params->gateway)))
				{
					std::cerr << "Gateway not in correct format.\n";
					return -1;
				}
				break;
			case 'n':
				if (!inet_pton(AF_INET, optarg, &(params->dnsServer)))
				{
					std::cerr << "DNS server not in correct format.\n";
					return -1;
				}
				break;
			case 'l':
				params->leaseTime = (strtol(optarg, &endptr, 0));
				if (errno == EINVAL || optarg == endptr)
				{
					std::cerr << "Invalid lease time value.\n";
					return -1;
				}
				break;
			case 'd':
				params->domain = optarg;
				break;
			default:
				std::cerr << "Unknown option.\n";
				return -1;
		}
	}
	if (params->startingIp == 0 || params->endingIp == 0)
	{
		std::cerr << "IP pool parameter is mandatory.\n";
		return -1;
	}
	if (params->gateway == 0)
	{
		std::cerr << "Gateway parameter is mandatory.\n";
		return -1;
	}
	if (params->dnsServer == 0)
	{
		std::cerr << "DNS server parameter is mandatory.\n";
		return -1;
	}
	if (params->leaseTime == 0)
	{
		std::cerr << "Lease time parameter is mandatory.\n";
		return -1;
	}
	if (params->domain == NULL)
	{
		std::cerr << "Domain parameter is mandatory.\n";
		return -1;
	}
	if (params->interface.empty())
	{
		std::cerr << "Interface parameter is mandatory.\n";
		return -1;
	}
	return 0;
}

/* function that parses the user ip pool input into a vector of available ip addresses */
void parsePool(std::vector<clientsList>* clients, serverParams params) 
{
	u_int32_t start, end;
	start = ntohl(params.startingIp);
	end = ntohl(params.endingIp);
	for (int i = start; i <= end; i++)
	{
		clientsList c;
		c.isOffered = c.isAcked = false;
		c.ip = htonl(i);
		clients->push_back(c);
	}
}

/* function that gets an available IP */
ipv4_addr getAddress(std::vector<clientsList>* clients, protocolIp proposed, bool isOffered)
{
	ipv4_addr ip = proposed.proposedIp;
	//search for IP that was offered to a victom before (creating an ACK)
	auto it = find_if(clients->begin(), clients->end(), [&ip](const clientsList& obj) {return (obj.ip == ip) && !(obj.isAcked);});
	if (it != clients->end())
	{
		if (!isOffered && it->isOffered)
		{
			memcpy(it->mac, proposed.mac, ETHER_ADDR_LEN);
			it->isAcked = !isOffered;
			it->offeredTime = std::chrono::system_clock::now();
			return it->ip;
		}		
	}
	//search for the first free IP
	for (auto i = clients->begin(); i != clients->end(); ++i)
	{	
		if (i->isOffered || i->isAcked) {
			continue;
		}
		memcpy(i->mac, proposed.mac, ETHER_ADDR_LEN);	
		i->isOffered = true;
		i->isAcked = !isOffered;
		i->offeredTime = std::chrono::system_clock::now();
		return i->ip;		
	}
	return 0;
}

int main(int argc, char *argv[]) {
	serverParams params;
	if (parseArgs(argc, argv, &params) == -1)
		return -1;	
	char errbuf[PCAP_ERRBUF_SIZE];
	signal(SIGINT, siginthandler);
	pcap_handle = pcap_open_live(params.interface.c_str(), BUFSIZ, 0, 10, errbuf);
	if (pcap_handle == NULL)
	{
		std::cerr << "Couldn't open interface " << errbuf << std::endl;
		return -1;
	}
	//create an object that will handle all the packet sniffing and sending
	Packet packet(pcap_handle);
	std::vector<protocolIp> protocol;
	std::vector<clientsList> clients;
	parsePool(&clients, params);
	// new thread will listen to incoming packets
	std::thread listener(&Packet::serverListener, &packet, params);
	// other thread will create answers
	while (1)
	{		
		(packet.m).lock();
		if (!packet.protocol.empty()) 
		{
			protocolIp request = packet.protocol.back();
			if (request.messageType == DHCP_DISCOVER || request.messageType == DHCP_REQUEST) {
				request.proposedIp = getAddress(&clients, request, request.messageType == DHCP_DISCOVER);
				packet.protocol.pop_back();
				(packet.m).unlock();			
				packet.createPacket(request.messageType == DHCP_DISCOVER ? DHCP_OFFER : DHCP_ACK, request);
				packet.sendPacket();
			}
			else {
				packet.protocol.pop_back();
				(packet.m).unlock();		
			}
			refreshPool(&clients, params.leaseTime);

		} else
			(packet.m).unlock();	
	}	
	listener.join();	
	return 0;
} 
