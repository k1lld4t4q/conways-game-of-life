CC=x86_64-w64-mingw32-gcc
CFLAGS=-Iinclude -I/usr/x86_64-w64-mingw32/include -c
LIBFLAGS=-Llib -lSDL2 -lSDL2main

all: main.o utils.o
	$(CC) -o gol main.o utils.o $(LIBFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) $(LIBFLAGS) main.c

utils.o: utils.c
	$(CC) $(CFLAGS) $(LIBFLAGS) utils.c

clean:
	rm *.o
	rm gol.exe
