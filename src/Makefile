#生成静态库
ifndef $(ver)
ver=
endif

CC=g++
#ar:生成静态库命令
AR=ar
CXXFLAGS =-std=c++11 -Wall -g  $(INCPATH) 
#其他头文件
INCPATH=-I ../h
#输出库的位置
OUTLIBPATH=../lib/
#wildcard通配符，获取当前目录下所有cpp文件
SRCS := $(wildcard *.cpp)
#cpp文件全部转为.o文件
OBJS := $(patsubst %.cpp, %.o, $(SRCS)) 
#库文件名libhahs.a
LIBNAME=$(OUTLIBPATH)/libhahs$(ver).a

#目标 :  生成目标所需.o文件
$(LIBNAME): $(OBJS)
	$(AR) cr $(LIBNAME) $(OBJS)

all: $(LIBNAME)

clean:
	-rm -f $(LIBNAME) $(OBJS)




