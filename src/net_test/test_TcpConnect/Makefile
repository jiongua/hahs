CC=g++
INCPATH= -I ../../
CXXFLAGS += -std=c++11 -Wall -g $(INCPATH) 
OUTLIBPATH=../../../lib
#库名
LIBNAME=$(OUTLIBPATH)/libhahs.a
#SRCS := $(wildcard *.cpp)
SRCS=tcpConnection_unittest.cpp
#OBJS := $(patsubst %.cpp, %.o, $(SRCS))
PROGNAME=test

all: 
	$(CC) $(CXXFLAGS) -o  $(PROGNAME) $(SRCS) $(LIBNAME) 
clean:
	-rm -f *.o $(PROGNAME)

