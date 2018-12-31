PROGS = mkfont
OBJ  = mkfont.o 
OBJS  = $(OBJ) ../../obj/mpqwrapper.cpp.o ../../obj/mpq.cpp.o ../../obj/pcx.cpp.o ../../obj/mylist.cpp.o \
			   ../../obj/auxil.cpp.o ../../obj/debug.cpp.o
SOURCES = mkfont.cpp

GRPLIB =  ../../obj/libs/grplib/src/grp.cpp.o \
          ../../obj/libs/grplib/src/cpp/font.cpp.o \
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


INCLUDE = -I../../include -I../../include/libs/grplib -I../../include/libs/mpqlib
CFLAGS = -g3 -funsigned-char -O0 $(INCLUDE)
LDFLAGS = -lstdc++ -lz -lm

all:  $(PROGS) $(OBJS)

$(PROGS): $(OBJS)
	gcc $(OBJS) -o $(PROGS) $(LDFLAGS) $(GRPLIB) $(MPQLIB)

$(OBJ): $(SOURCES)
	gcc -c $(CFLAGS) $(SOURCES) -o $(OBJ)
clean:
	-rm $(PROGS) $(OBJ) *.fnt
	
