CC = g++
CFLAGS = -c -Wall
SOURCES = main.cpp Stack.cpp Hash.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = $(SOURCES:.cpp=.h)

all: $(SOURCES) main


main: $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@

.cpp.o: $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o

clean.exe:
	rm -rf *.exe

