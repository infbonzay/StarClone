
OBJS = ../../obj/wmem.o ../../obj/network_general.o ../../obj/network_udp.o \
     ../../obj/network_tcp.o ../../obj/network_dc.o ../../obj/network_modem.o
INCLUDE = -I../../include -I../../include/network -DWITHMPQLIB
CFLAGS = -g3 -funsigned-char -O0 $(INCLUDE)
LDFLAGS = -lstdc++ 

all: sender receiver
	
	

sender: sender.o ../../obj/wmem.o ../../obj/network_general.o ../../obj/network_udp.o \
	../../obj/network_tcp.o ../../obj/network_dc.o ../../obj/network_modem.o 
	gcc $(LDFLAGS) $(OBJS) sender.o -o sender

receiver: receiver.o ../../obj/wmem.o ../../obj/network_general.o ../../obj/network_udp.o \
	../../obj/network_tcp.o ../../obj/network_dc.o ../../obj/network_modem.o 
	gcc $(LDFLAGS) $(OBJS) receiver.o -o receiver

sender.o: sender.cpp ../../src/network/network_udp.cpp
	gcc -c $(CFLAGS) sender.cpp -o sender.o

receiver.o: receiver.cpp ../../src/network/network_udp.cpp
	gcc -c $(CFLAGS) receiver.cpp -o receiver.o

../../obj/wmem.o: ../../src/wmem.cpp 
	gcc -c $(CFLAGS) ../../src/wmem.cpp -o ../../obj/wmem.o

../../obj/network_general.o: ../../src/network/network_general.cpp 
	gcc -c $(CFLAGS) ../../src/network/network_general.cpp -o ../../obj/network_general.o

../../obj/network_udp.o: ../../src/network/network_udp.cpp 
	gcc -c $(CFLAGS) ../../src/network/network_udp.cpp -o ../../obj/network_udp.o

../../obj/network_tcp.o: ../../src/network/network_tcp.cpp 
	gcc -c $(CFLAGS) ../../src/network/network_tcp.cpp -o ../../obj/network_tcp.o

../../obj/network_dc.o: ../../src/network/network_dc.cpp 
	gcc -c $(CFLAGS) ../../src/network/network_dc.cpp -o ../../obj/network_dc.o

../../obj/network_modem.o: ../../src/network/network_modem.cpp 
	gcc -c $(CFLAGS) ../../src/network/network_modem.cpp -o ../../obj/network_modem.o

clean:
	-rm sender receiver *.o
	
