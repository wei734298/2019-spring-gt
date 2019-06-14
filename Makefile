TARGET = CP_N26074914

SRC_DIR = ./src/
INC_DIR = ./include/ 
LIB_DIR = ./fake-mininet-master/


INC = -I${INC_DIR} -I./fake-mininet-master/lib/
LIB = -L${LIB_DIR}

SRC = $(wildcard $(SRC_DIR)*.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))

CXX = g++ -std=c++11
CXXFLAGS = -O3

${TARGET}: ${OBJ}
	${CXX} ${CXXFLAGS} ${OBJ} ${INC} ${LIB} -lfakemn -o ${TARGET}

$(SRC_DIR)%.o: $(SRC_DIR)%.cpp
	${CXX} ${CXXFLAGS} ${INC} -o $@ -c $<

.PHONY: clean
clean:
	rm -f core *~ ${SRC_DIR}*.o ${TARGET}
