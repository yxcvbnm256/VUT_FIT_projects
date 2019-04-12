CC=g++
CFLAGS=-std=c++11
LIBS=-pthread packet.cpp -lpcap

all: pds-dhcprogue pds-dhcpstarve


pds-dhcprogue: pds-dhcprogue.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

pds-dhcpstarve: pds-dhcpstarve.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

clean:
	rm -f *.o pds-dhcprogue pds-dhcpstarve
