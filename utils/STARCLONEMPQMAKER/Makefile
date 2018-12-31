
OBJS = makempq.o 
INCLUDE = -I../../include -I../../include/libs/grplib -I../../include/libs/mpqlib
CFLAGS = -g3 -funsigned-char -O0 $(INCLUDE) -DMPQLIB
LDFLAGS = -lstdc++ -lz -lm

GRPLIB =  ../../obj/libs/grplib/src/grp.cpp.o \
          ../../obj/libs/grplib/src/gr8.cpp.o \
          ../../obj/libs/grplib/src/cpp/font.cpp.o \
          ../../obj/libs/grplib/src/palette.cpp.o \
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

all: makempq mpqextractall mpqextractlistfile
	

makempq: makempq.o
	gcc $(LDFLAGS) $(OBJS) -o makempq $(GRPLIB) $(MPQLIB)

makempq.o: makempq.cpp
	gcc -c $(CFLAGS) makempq.cpp -o makempq.o

mpqextractall: mpqextractall.o
	gcc $(LDFLAGS) mpqextractall.o -o mpqextractall  $(GRPLIB) $(MPQLIB)

mpqextractall.o: mpqextractall.cpp
	gcc -c $(CFLAGS) mpqextractall.cpp -o mpqextractall.o

mpqextractlistfile: mpqextractlistfile.o
	gcc $(LDFLAGS) mpqextractlistfile.o -o mpqextractlistfile  $(GRPLIB) $(MPQLIB)

mpqextractlistfile.o: mpqextractlistfile.cpp
	gcc -c $(CFLAGS) mpqextractlistfile.cpp -o mpqextractlistfile.o

clean:
	-rm makempq makempq.o mpqextractall mpqextractall.o mpqextractlistfile mpqextractlistfile.o
	
	
