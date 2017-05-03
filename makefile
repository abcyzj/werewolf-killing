ver=debug

ifeq ($(ver), debug)
CXXFLAGS = -g -Ddebug -pthread
else
CXXFLAGS = -O3 -pthread
endif

all:serv_main client_main

serv_main:serv_main.o message.o client.o broadcast.o accept.o
	$(CXX) $(CXXFLAGS) -o $@ $^

client_main:client_main.o message.o for_client.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm *.o
