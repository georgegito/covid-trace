CC=g++
CFLAGS= -O3

default: compile

compile:
	$(CC) -o test test.cpp $(CFLAGS) -lpthread
	./test
	@printf "\n"
