CC=g++
INCPATH= -I ../
CXXFLAGS += -std=c++11 -Wall -g $(INCPATH) 
OUTLIBPATH=../../lib
#库名
LIBNAME=$(OUTLIBPATH)/libhahs.a
#SRCS := $(wildcard *.cpp)
SRCS=Buffer_test.cpp
#OBJS := $(patsubst %.cpp, %.o, $(SRCS))
PROGNAME=test

all: 
	$(CC) $(CXXFLAGS) -o  $(PROGNAME) $(SRCS) $(LIBNAME) 
	#g++ -std=c++11 -o test sockOps_test.cpp -I ../../ ../../../lib/libhahs.a

clean:
	-rm -f *.o $(PROGNAME)

