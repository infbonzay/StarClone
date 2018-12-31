
OBJS = ../../obj/wmem.cpp.o ../../obj/network/network_general.cpp.o ../../obj/network/network_udp.cpp.o \
     ../../obj/network/network_tcp.cpp.o ../../obj/network/network_dc.cpp.o ../../obj/network/network_modem.cpp.o \
     ../../obj/network/network_dedicated.cpp.o
INCLUDE = -I../../include -I../../include/network -I../../include/libs/grplib -I../../include/libs/mpqlib \
     -DWITHMPQLIB -DUNDERLINUX -DWITHNETUDP
CFLAGS = -g3 -funsigned-char -O0 $(INCLUDE)
LDFLAGS = -lstdc++ 

all: sender receiver
	

sender: sender.o 
	gcc $(LDFLAGS) $(OBJS) sender.o -o sender

receiver: receiver.o 
	gcc $(LDFLAGS) $(OBJS) receiver.o -o receiver

sender.o: sender.cpp 
	gcc -c $(CFLAGS) sender.cpp -o sender.o

receiver.o: receiver.cpp 
	gcc -c $(CFLAGS) receiver.cpp -o receiver.o

clean:
	-rm sender receiver *.o
	
