MSG_DIR = ./message
CLNT_DIR = ./for_client
SERV_DIR = ./for_server

INC_DIR = -I${MSG_DIR}\
					-I${SERV_DIR}\
					-I${CLNT_DIR}

SERV_SRC = ${wildcard	${MSG_DIR}/*.cpp}\
			${wildcard	${SERV_DIR}/*.cpp}

SERV_OBJ = ${patsubst %.cpp, %.o, ${SERV_SRC}}

CLNT_SRC = ${wildcard ${MSG_DIR}/*.cpp}\
					 ${wildcard ${CLNT_DIR}/*.cpp}

CLNT_OBJ = ${patsubst %.cpp, %.o, ${CLNT_SRC}}

# test:$(TARGET)
# 	@echo ${OBJ}

TARGET1 = serv_main.out
TARGET2 = clnt_main.out


ver=debug

ifeq ($(ver), debug)
CXXFLAGS = -g -Ddebug -pthread -std=c++11 $(INC_DIR)
else
CXXFLAGS = -O3 -pthread -std=c++11 $(INC_DIR)
endif

all:$(TARGET1) $(TARGET2)
	@echo "Compile done."


${TARGET1}: ${SERV_OBJ}
	$(CXX) ${SERV_OBJ} -o $@ $(CXXFLAGS)

${TARGET2}: ${CLNT_OBJ}
	$(CXX) $(CLNT_OBJ) -o $@ $(CXXFLAGS)

clean:
	@rm -f ${SERV_OBJ} ${CLNT_OBJ}
	@echo "Clean done."
