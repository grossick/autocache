.PHONY: debug all autocache.o

all: debug libautocache.a autocache.so

debug:
	$(CC) -ggdb main.c autocache.c -o $@

libautocache.a: autocache.o
	ar rcs libautocache.a autocache.o

autocache.o:
	$(CC) -c -fPIC autocache.c -o $@

autocache.so: autocache.o
	$(CC) $^ -shared -o libautocache.so
