BITS=32
PLATFORM = -m$(BITS)
CFLAGS = $(PLATFORM) -DWITHMPQLIB -ansi -g3 -O0 -I../../include -I../../include/smacker -fomit-frame-pointer -frename-registers -funroll-loops
LDFLAGS = $(PLATFORM)  -lSDL -lpthread -lgrp -L./grplib -lstdc++ -lmpq -lz

all:  bin/smkview$(BITS) obj/smkview.o ../../obj/smk_bitstream.o ../../obj/smk_hufftree.o \
    ../../obj/mpqwrapper.o obj/audiolowlevel.o

bin/smkview$(BITS): obj/smkview.o ../../obj/smacker.o ../../obj/smk_bitstream.o \
    ../../obj/smk_hufftree.o ../../obj/mpqwrapper.o obj/audiolowlevel.o
	gcc $(LDFLAGS) obj/smkview.o ../../obj/smacker.o ../../obj/smk_bitstream.o \
    ../../obj/smk_hufftree.o ../../obj/mpqwrapper.o obj/audiolowlevel.o -o bin/smkview$(BITS)

obj/smkview.o: src/smkview.cpp
	gcc -c $(CFLAGS) src/smkview.cpp -o obj/smkview.o
../../obj/smacker.o: ../../src/smacker/smacker.cpp
	gcc -c $(CFLAGS) ../../src/smacker/smacker.cpp -o ../../obj/smacker.o
../../obj/smk_bitstream.o: ../../src/smacker/smk_bitstream.cpp
	gcc -c $(CFLAGS) ../../src/smacker/smk_bitstream.cpp -o ../../obj/smk_bitstream.o
../../obj/smk_hufftree.o: ../../src/smacker/smk_hufftree.cpp
	gcc -c $(CFLAGS) ../../src/smacker/smk_hufftree.cpp -o ../../obj/smk_hufftree.o
../../obj/mpqwrapper.o: ../../src/mpqwrapper.cpp
	gcc -c $(CFLAGS) ../../src/mpqwrapper.cpp -o ../../obj/mpqwrapper.o
obj/audiolowlevel.o: ../../src/sound.null/audiolowlevel.cpp
	gcc -c $(CFLAGS) ../../src/sound.null/audiolowlevel.cpp -o obj/audiolowlevel.o
    
clean:
	-rm bin/smkview32 bin/smkview64 obj/*.o

