CC=g++
SOURCE=imapcl.cpp main.cpp
EXECUTABLE=imapcl
LFLAGS= -lssl -lcrypto 
CFLAGS = -std=c++11 -Wall -Wextra -pedantic

all: 
	$(CC) $(CFLAGS) $(SOURCE) $(LFLAGS) -o $(EXECUTABLE)

pack:
	tar -cvf xpawlu00.tar imapcl.cpp imapcl.h main.h main.cpp Makefile README manual.pdf
clean:
	rm -f imapcl
