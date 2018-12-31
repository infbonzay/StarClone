CFLAGS = -DWITHMPQLIB -ansi -g3 -O0 -I../../include/libs/mpqlib -I../../include/libs/grplib \
         -I../../include -I../../include/smacker \
         -fomit-frame-pointer -frename-registers -funroll-loops
LDFLAGS = -lSDL -lpthread -lstdc++ -lz -lm -lSDL_mixer

OBJS = ../../obj/smacker/smacker.cpp.o \
       ../../obj/smacker/smk_bitstream.cpp.o \
       ../../obj/smacker/smk_hufftree.cpp.o \
       ../../obj/mpqwrapper.cpp.o \
       ../../obj/audio/sdl/audiolib.cpp.o

GRPLIB =  ../../obj/libs/grplib/src/grp.cpp.o \
          ../../obj/libs/grplib/src/gr8.cpp.o \
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


all:  bin/smkview obj/smkview.o

bin/smkview: obj/smkview.o
	gcc $(LDFLAGS) obj/smkview.o $(OBJS) $(GRPLIB) $(MPQLIB) -o bin/smkview

obj/smkview.o: src/smkview.cpp
	gcc -c $(CFLAGS) src/smkview.cpp -o obj/smkview.o
    
clean:
	-rm bin/smkview* obj/*.o
