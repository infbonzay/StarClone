PROGS = testmylist
OBJ  = testmylist.o
OBJS  = $(OBJ) ../../obj/mylist.cpp.o
SOURCES = testmylist.cpp

INCLUDE = -I../../include
CFLAGS = -g3 -funsigned-char -O0 $(INCLUDE)
LDFLAGS = -lstdc++

all:  $(PROGS) $(OBJS)

$(PROGS): $(OBJS)
	gcc $(OBJS) -o $(PROGS) $(LDFLAGS)

$(OBJ): $(SOURCES)
	gcc -c $(CFLAGS) $(SOURCES) -o $(OBJ)

clean:
	-rm $(PROGS) $(OBJ)
	
