ver=debug

ifeq ($(ver), debug)
CXXFLAGS = -c -g -Ddebug
else
CXXFLAGS = -c -O3
endif

all: test_socket_server test_socket_clnt

test_socket_server: test_socket_server.o message.o
	$(CXX) -o $@ $^

test_socket_clnt: test_socket_clnt.o message.o
	$(CXX) -o $@ $^

clean:
	rm *.o
