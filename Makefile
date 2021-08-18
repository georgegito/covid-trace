CC=gcc
CFLAGS= -O3

BUILD_DIR=build
SRC_DIR=src
INCLUDE_DIR=./include
SOURCES := $(shell find $(SRC_DIR) -name '*.c')
READER_DIR=reader/reader.c

$(info $(shell mkdir -p $(BUILD_DIR)))

default: compile

compile:
	$(CC) -o $(BUILD_DIR)/main -I$(INCLUDE_DIR) $(SOURCES) $(CFLAGS) -lpthread
	./build/main
	@printf "\n"

pi:
	arm-linux-gnueabihf-gcc -o $(BUILD_DIR)/main_pi -I$(INCLUDE_DIR) $(SOURCES) -lpthread
	@printf "\n"

read:
	$(CC) -o $(BUILD_DIR)/reader -I$(INCLUDE_DIR) $(READER_DIR) $(CFLAGS) -lpthread
	./build/reader
	@printf "\n"