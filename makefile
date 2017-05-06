ver=debug

ifeq ($(ver), debug)
CXXFLAGS = -g -Ddebug -pthread -std=c++11
else
CXXFLAGS = -O3 -pthread -std=c++11
endif

all:serv_main.out client_main.out

serv_main.out:serv_main.o message.o client.o broadcast.o accept.o
	$(CXX) $(CXXFLAGS) -o $@ $^

client_main.out:client_main.o message.o for_client.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm *.o
