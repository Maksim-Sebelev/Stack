CC = g++
CFLAGS = -c
FILES_CPP = main.cpp StackFunc.cpp HashFunc.cpp
OBJECTS = $(FILES_CPP:.cpp=.o)
HEADERS = $(FILES_CPP:.cpp=.h)

all: $(SOURCES) main


main: $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@

.cpp.o: $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o

clean.exe:
	rm -rf *.exe

