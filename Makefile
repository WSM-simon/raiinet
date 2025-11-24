SRC = src
INC = include

CXX = g++
CXXFLAGS = -std=c++14 -Wall -I${INC}

IMPLS = $(wildcard src/*-impl.cc src/*/*-impl.cc)
OBJS = $(IMPLS:.cc=.o) src/main.o

EXEC = raiinet

${EXEC}: ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} -o ${EXEC}

clean:
	rm -f ${OBJS} ${EXEC}