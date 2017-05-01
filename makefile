ver=debug

ifeq ($(ver), debug)
CXXFLAGS = -g -Ddebug -pthread
else
CXXFLAGS = -O3 -pthread
endif

all:ac_main

ac_main:ac_main.o message.o client.o broadcast.o accept.o
		$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm *.o
