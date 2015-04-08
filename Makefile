CC= gcc
CFLAGS= -g -Wall -std=c99
LIBS = -lcurl -lpanel -lmenu -lncurses -ljansson

all: staq

staq: staq.o stackexchange.o display.o se_query_builder.o
	$(CC) $^ -o $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $? -o $@

clean:
	rm -f *.o

