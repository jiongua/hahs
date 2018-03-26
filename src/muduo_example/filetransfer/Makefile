CC=g++
INCPATH= -I ../../
CXXFLAGS += -std=c++11 -Wall -g $(INCPATH) -pthread
OUTLIBPATH=../../../lib
LIBNAME=$(OUTLIBPATH)/libhahs.a
#SRCS := $(wildcard *.cpp)
#SRCS=transferServer.cpp 
#OBJS := $(patsubst %.cpp, %.o, $(SRCS))
OBJS=transferServer.o transferClient.o trans.o
PROGNAME=trans

all: $(PROGNAME)

$(PROGNAME): $(OBJS)
	$(CC) $(CXXFLAGS) -o  $(PROGNAME) $(OBJS) $(LIBNAME)

clean:
	-rm -f *.o $(PROGNAME)

