CC = g++
CFLAGS = -Wall -I/usr/local/include -c
LFLAGS = -Wall -L/usr/local/lib -lgdal

catalogdump: catalogdump.cpp
	$(CC) $(CFLAGS) catalogdump.cpp
	$(CC) $(LFLAGS) catalogdump.o -o catalogdump
