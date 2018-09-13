# SSD Simulator
# 2017 ZDV, Mainz. All Rights Reserved

CC = g++

#compiler flags:
# -g adds debugging information to the executable file
# -Wall turns on most, but not all. compiler warnings
CFLAGS = -g -std=c++11 -Wall -D_REENTRANT

#include libs
LDLIB = -g

DEP = ssd_config.hh
SRC = main.cpp ssd_config.cpp ssd.cpp page.cpp block.o plane.o die.o
OBJ = main.o ssd_config.o ssd.o page.o block.o plane.o die.o

main: main.o ssd_config.o ssd.o page.o block.o
	$(CC) $(LDFLAGS) $(LDLIBS) -o main $(OBJ)

main.o: $(SRC)
	$(CC) $(CFLAGS) -c main.cpp

ssd_config.o: ssd_config.hh ssd_config.cpp
	$(CC) $(CFLAGS) -c ssd_config.cpp

ssd.o: ssd.hh ssd.cpp
	$(CC) $(CFLAGS) -c ssd.cpp

die.o: die.hh die.cpp
	$(CC) $(CFLAGS) -c die.cpp

plane.o: plane.hh plane.cpp
	$(CC) $(CFLAGS) -c plane.cpp

block.o: block.hh block.cpp
	$(CC) $(CFLAGS) -c block.cpp

page.o: page.hh page.cpp
	$(CC) $(CFLAGS) -c page.cpp

clean:
	rm *.o
	rm main
