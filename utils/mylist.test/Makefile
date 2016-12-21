PROGS = testmylist
OBJ  = testmylist.o
OBJS  = $(OBJ) mylist.o
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
mylist.o: ../../src/mylist.cpp
	gcc -c $(CFLAGS) ../../src/mylist.cpp -o mylist.o
clean:
	-rm $(PROGS) $(OBJ)
	
