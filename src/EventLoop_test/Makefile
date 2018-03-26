#g++ -std=c++11 -o test sockOps_test.cpp -I ../../ ../../../lib/libhahs.a

CC=g++
#find SocketOps.h
INCPATH= -I ../
CXXFLAGS += -std=c++11 -Wall -g $(INCPATH) -pthread
OUTLIBPATH=../../lib
#库名
LIBNAME=$(OUTLIBPATH)/libhahs.a
#SRCS := $(wildcard *.cpp)
SRCS=EventLoop_test2.cpp
#OBJS := $(patsubst %.cpp, %.o, $(SRCS))
PROGNAME = test

all: 
	$(CC) $(CXXFLAGS) -o  $(PROGNAME) $(SRCS) $(LIBNAME) 

clean:
	-rm -f *.o $(PROGNAME)

