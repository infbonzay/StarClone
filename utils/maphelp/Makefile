PROGS = maphelp
OBJ  = maphelp.o
OBJS  = $(OBJ)
SOURCES = maphelp.cpp

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
	
