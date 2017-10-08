PROGS = maketbl
OBJ  = maketbl.o
OBJS  = $(OBJ) ../../obj/tbl.o ../../obj/debug.o ../../obj/wmem.o ../../obj/auxil.o ../../obj/mpqwrapper.o \
	../../obj/mpq.o ../../obj/mylist.o ../../obj/network_general.o  ../../obj/network_udp.o  \
	 ../../obj/network_tcp.o  ../../obj/network_dc.o ../../obj/network_modem.o ../../obj/stringfiles.o
SOURCES = maketbl.cpp

INCLUDE = -I../../include
CFLAGS = -g3 -funsigned-char -O0 $(INCLUDE)
LDFLAGS = -lSDL -lpthread -lgrp -L./grplib -lstdc++ -lmpq -lz

all:  $(PROGS) $(OBJS)

$(PROGS): $(OBJS)
	gcc $(OBJS) -o $(PROGS) $(LDFLAGS)

$(OBJ): $(SOURCES)
	gcc -c $(CFLAGS) $(SOURCES) -o $(OBJ)

clean:
	-rm $(PROGS) $(OBJ)
	
