CC= gcc
CFLAGS= -g -Wall
LIBS = -lcurl -lpanel -lmenu -lncurses -ljansson

all: staq

staq: staq.o stackexchange.o display.o se_query_builder.o renderMarkdown.o libsoldout/libsoldout.o
	$(CC) $(LIBS) $^ -o $@

# phony up, so make doesn't think this is a file
.PHONY: libsoldout.o

# FIXME this is probably the wrong way to specify this target
libsoldout/libsoldout.o:
	$(MAKE) -C libsoldout

# uhh, add markdown thing as separate item?
# renderMarkdown.o: renderMarkdown.c

.c.o:
	$(CC) $(CFLAGS) -c $? -o $@

clean:
	rm -f *.o
	$(MAKE) -C libsoldout clean

