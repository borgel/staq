CC= gcc
CFLAGS= -g -Wall
LIBS = -lcurl -lncurses -ljansson

all: staq

staq: staq.o stackexchange.o se_query_builder.o
	$(CC) $(LIBS) $^ -o $@

.c.o:
	$(CC) $(CFLAGS) -c $? -o $@

clean:
	rm -f *.o

