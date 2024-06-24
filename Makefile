CC=g++
CFLAGS=-Wall -Wextra -pedantic -std=c++17
TARGET=cacheSim

all: $(TARGET)

$(TARGET): main.cpp
	$(CC) $(CFLAGS) -o $(TARGET) main.cpp

clean:
	rm -f $(TARGET)