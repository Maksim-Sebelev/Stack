CC = g++
CFLAGS = -c
SOURCES = main.cpp StackFunc.cpp HashFunc.cpp
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

