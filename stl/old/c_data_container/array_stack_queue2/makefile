# 目标声明
.PHONY: all release clean install check

# 默认宏赋值
#CC := gcc
CXX := g++
#AR := ar
# 编译选项[[注意: CPPFLAGS = C预处理器参数(C 和Fortran 编译器也会用到)]]
# c 编译选项(gcc 特有)
#CFLAGS += -O2 -std=c11
# c/c++ 共有
#CPPFLAGS += -O2 -std=c++0x
# c++ 特有
CXXFLAGS += -O2 -std=c++0x
# 链接器参数(如:“ld”)
LDFLAGS += -lpthread
# 追加宏定义(一般用来禁用assert())
DEFS += -DNDEBUG
# 目标文件, := 实际上等于 =, 是一个赋值操作
TARGET := test_arr_as_aq
# 依赖文件
objects := _array_queue.o _array_stack.o test_main.o

first:
	@echo ""
	@echo "passed, please make a choose target: make [target]"
	@echo "[target]::{all release clean install check}"
	@echo "demo:"
	@echo "make all"
	@echo ""
# 隐晦规则x3 (未实现自动搜索的隐晦规则)
# 对于.c 文件自动$(CC) –c $(CFLAGS) $(CPPFLAGS)
# 对于.cc 文件自动$(CXX) –c $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS)
# c++ 不用找多余文件.h, c 可能需要重新赋值...
_array_queue.o:

_array_stack.o:

test_main.o:

# 通配符只能作用于.c/.cpp, 不能作用于.o, 
# 因为.o 还未生成, makefile 顺序读下来会找不到的
# 加上隐晦规则, .o 只能自己控制...
#all_c := *.c
all_cpp := *.cpp



# 拦截游标, 硬性添加调试选项
all:#CFLAGS += -g3 -Wall
	CXXFLAGS += -g3 -Wall
all:
	@echo "building all cc with < 隐晦规则 x3 > and gdb -g3"
	@echo "start link all .o"
	#$(CC) -c *.c
	$(CXX) -c *.cpp $(CXXFLAGS)
	$(CXX) -o $(TARGET) *.o $(LDFLAGS)
	@echo "make target finish"
	
release:
	@echo "building all cc with < 隐晦规则 x3 >"
	@echo "start link all .o"
	#$(CC) -c *.c
	$(CXX) -c *.cpp $(CXXFLAGS) $(DEFS)
	$(CXX) -o $(TARGET) *.o $(LDFLAGS)
	@echo "make target finish"
	
install:
	@echo "install needs root authority"

check:
	@echo "make checking"
	#gcc -MM $(all_c)
	# -MM 可以检查依赖关系, 自动搜索当前文件夹下所有的*.cpp 文件
	# 但是如果没有通配成功就会报错, 所以至少有1 个文件, 否则删掉这句
	g++ -MM $(all_cpp)
clean:
	rm $(TARGET) *.o
	@echo "clean up finish"
	



