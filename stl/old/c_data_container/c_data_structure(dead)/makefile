# 目标声明
.PHONY: help all release install check clean lib

# 默认宏赋值
CC := gcc
CXX := g++

# 编译选项[[注意: CPPFLAGS = C预处理器参数(C 和Fortran 编译器也会用到)]]
# c 编译选项(gcc 特有)
CFLAGS += -O2 -std=c11
#
# c/c++ 共有(由于c 和cpp 编译器可能会发生混编, 所以保留c or cpp 分立的局面...保留机动性)
#CPPFLAGS += -O2 -std=c++0x
#
# c++ 特有
CXXFLAGS += -O2 -std=c++0x
#
# 链接器参数(如:“-lld, -lpthread 等”)
LDFLAGS += -lpthread

# 追加宏定义(一般用来禁用assert(), 指定POSIX 版本)
# posix 版本, 你不指定的话, gcc 默认也是使用posix 版本的标准, 而不是gnu 编译器标准
DEFS += -DNDEBUG -D_POSIX_SOURCE

#***
# 目标文件, := 实际上等于 =, 是一个赋值操作
TARGET := obj_out
#***

#>>>>>
objects += *.o 
#cpp_src += ./src/no_assert/*.cpp ./src/make_project/*.cpp
c_src := ./c_linklist_str/*.c ./c_sort/*.c *.c
#<<<<<

help:
	@echo ""
	@echo "passed, please make a choose target: make [target]"
	@echo "[target]::{help all release install check clean lib}"
	@echo "demo:"
	@echo "make all"
	@echo ""
	@echo ""

# 拦截游标, 硬性添加调试选项
all:CFLAGS += -g3 -Wall
	CXXFLAGS += -g3 -Wall
all:
	@echo ""
	@echo ">>building all src with [-g3 -Wall]<<"
	$(CC) -c $(c_src) $(CFLAGS) $(DEFS)
	#$(CXX) -c $(cpp_src) $(CXXFLAGS) $(DEFS)
	$(CXX) -o $(TARGET) $(objects) $(LDFLAGS)
	#$(CC) -o $(TARGET) $(objects) $(LDFLAGS)
	@echo ">>make target-all finish<<"
	@echo ""
	@echo ""
	
release:
	@echo ""
	@echo ">>building all src with [release]<<"
	$(CC) -c $(c_src) $(CFLAGS) $(DEFS)
	#$(CXX) -c $(cpp_src) $(CXXFLAGS) $(DEFS)
	$(CXX) -o $(TARGET) $(objects) $(LDFLAGS)
	$(CC) -o $(TARGET) $(objects) $(LDFLAGS)
	@echo ">>make target-release finish<<"
	@echo ""
	@echo ""
	
install:
	@echo ""
	@echo ">>install needs root authority<<"
	@echo ""
	@echo ""

check:
	@echo ""
	@echo ">>all source file<<"
	@echo $(c_src)
	#@echo $(cpp_src)
	@echo ""
	@echo ">>checking all src<<"
	gcc -MM $(c_src)
	#g++ -MM $(cpp_src)
	@echo ""
	@echo ""

clean:
	@echo ""
	rm $(TARGET) $(objects)
	@echo ">>clean up finish<<"
	@echo ""
	@echo ""
	

