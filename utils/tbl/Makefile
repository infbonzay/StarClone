CC = g++ #clang++
PROGS = maketbl
OBJ  = maketbl.o
OBJS  = $(OBJ) ../../obj/tbl.cpp.o ../../obj/debug.cpp.o ../../obj/wmem.cpp.o ../../obj/auxil.cpp.o \
               ../../obj/mpqwrapper.cpp.o  ../../obj/network/network_udp.cpp.o  \
               ../../obj/mpq.cpp.o ../../obj/mylist.cpp.o ../../obj/network/network_general.cpp.o \
               ../../obj/network/network_tcp.cpp.o ../../obj/network/network_dedicated.cpp.o  \
               ../../obj/network/network_dc.cpp.o ../../obj/network/network_modem.cpp.o ../../obj/stringfiles.cpp.o

GRPLIB =  ../../obj/libs/grplib/src/grp.cpp.o \
          ../../obj/libs/grplib/src/gr8.cpp.o \
          ../../obj/libs/grplib/src/cpp/grpcpp.cpp.o

MPQLIB =  ../../obj/libs/mpqlib/src/SCommon.cpp.o \
          ../../obj/libs/mpqlib/src/SCompression.cpp.o \
          ../../obj/libs/mpqlib/src/SFileCompactArchive.cpp.o \
          ../../obj/libs/mpqlib/src/SFileCreateArchiveEx.cpp.o \
          ../../obj/libs/mpqlib/src/SFileExtractFile.cpp.o \
          ../../obj/libs/mpqlib/src/SFileFindFile.cpp.o \
          ../../obj/libs/mpqlib/src/SFileOpenArchive.cpp.o \
          ../../obj/libs/mpqlib/src/SFileOpenFileEx.cpp.o \
          ../../obj/libs/mpqlib/src/SFileReadFile.cpp.o \
          ../../obj/libs/mpqlib/src/SListFile.cpp.o \
          ../../obj/libs/mpqlib/src/StormPortLinux.cpp.o \
          ../../obj/libs/mpqlib/pklib/crc32.cpp.o \
          ../../obj/libs/mpqlib/pklib/explode.cpp.o \
          ../../obj/libs/mpqlib/pklib/implode.cpp.o \
          ../../obj/libs/mpqlib/huffman/huffman.cpp.o \
          ../../obj/libs/mpqlib/wave/wave.cpp.o 
 
SOURCES = maketbl.cpp

INCLUDE = -I../../include -I../../include/libs/grplib -I../../include/libs/mpqlib
CFLAGS = -g3 -O0 $(INCLUDE)
LDFLAGS = -lSDL -lpthread -lstdc++ -lz -lm

all:  $(PROGS) $(OBJS)

$(PROGS): $(OBJS)
	$(CC) $(OBJS) $(GRPLIB) $(MPQLIB) -o $(PROGS) $(LDFLAGS)

$(OBJ): $(SOURCES)
	$(CC) -c $(CFLAGS) $(SOURCES) -o $(OBJ)

clean:
	-rm $(PROGS) $(OBJ)
	
