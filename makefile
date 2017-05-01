ver=debug

ifeq ($(ver), debug)
CXXFLAGS = -g -Ddebug -pthread
else
CXXFLAGS = -O3 -pthread
endif

all:br_main

br_main:br_main.o broadcast.o message.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm *.o
