PROGS = mkfont
OBJ  = mkfont.o 
OBJS  = $(OBJ) ../../obj/mpqwrapper.o ../../obj/mpq.o ../../obj/pcx.o ../../obj/mylist.o ../../obj/auxil.o
SOURCES = mkfont.cpp

INCLUDE = -I../../include
CFLAGS = -g3 -funsigned-char -O0 $(INCLUDE)
LDFLAGS = -lstdc++ -lgrp -lmpq -lz -lm

all:  $(PROGS) $(OBJS)

$(PROGS): $(OBJS)
	gcc $(OBJS) -o $(PROGS) $(LDFLAGS)

$(OBJ): $(SOURCES)
	gcc -c $(CFLAGS) $(SOURCES) -o $(OBJ)
clean:
	-rm $(PROGS) $(OBJ) *.fnt
	
