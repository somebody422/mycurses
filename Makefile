
CC=g++
TARGET=mycurses
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(patsubst src/%.cpp,obj/%.o,$(SOURCES))
FLAGS=-Wall -std=c++11

all: $(TARGET)

run: all
	./$(TARGET).exe

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(FLAGS) -lncurses

$(OBJECTS): obj/%.o: src/%.cpp src/mycurses.hpp
	$(CC) -c -o $@ $< -I . $(FLAGS)

test: test.cpp
	$(CC) -o test test.cpp $(filter-out obj/main.o,$(OBJECTS)) $(FLAGS) -lncurses
	./$@.exe

clean:
	rm obj/*.o; \
	rm $(TARGET);
