CC=g++
CFLAGS= -O3

default: compile

compile:
	$(CC) -o covid_trace covid_trace.cpp $(CFLAGS) -lpthread
	./covid_trace
	@printf "\n"
