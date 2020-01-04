CC 			= g++
CFLAGS 		= -W -Wall
INCLUDES 	= -I./include/
LDFLAGS 	= -lSDL2
SOURCE_DIR 	= ./source
BINARY_DIR  = ./bin

all : clean main run

main:
	$(CC) $(SOURCE_DIR)/main.cpp $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $(BINARY_DIR)/main

run:
	$(BINARY_DIR)/main

clean:
	rm -f $(BINARY_DIR)/main