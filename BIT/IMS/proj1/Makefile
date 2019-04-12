CC=g++
SOURCE=hasici.cc hasici.h
EXECUTABLE=hasici
LFLAGS= -lsimlib -lm 
CFLAGS = -std=c++11 -g -O2

all: 
	$(CC) $(CFLAGS) $(SOURCE) $(LFLAGS) -o $(EXECUTABLE)

pack:
	tar -cvzf 05_xpawlu00_xstude21.tar.gz Makefile hasici.cc hasici.h dokumentace.pdf
clean:
	rm -f hasici
run:
	./$(EXECUTABLE)
