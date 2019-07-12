# new make file template
#
# gdb 调试的中间文件集
gdb_temp_files = v1_array.o v2_array.o v3_array.o test_Vx_array.o

# 编译选项
build_options = -l pthread -l dl -l m

# 编译器选择, 可能是gcc or clang, 不支持win!!
# win 不能使用rm, 而且win shell 跟unix shell 差别很大, 不可能兼容的
# cc = compiler
cc = g++

test_Vx_array: $(gdb_temp_files)
	$(cc) -o test_Vx_array $(gdb_temp_files) $(build_options)

v1_array.o: v1_array.h v1_array.cpp
	$(cc) -c v1_array.h v1_array.cpp $(build_options)

v2_array.o: v2_array.h v2_array.cpp
	$(cc) -c v2_array.h v2_array.cpp $(build_options)

v3_array.o: v3_array.h v3_array.cpp
	$(cc) -c v3_array.h v3_array.cpp $(build_options)

test_Vx_array.o: test_Vx_array.cpp
	$(cc) -c test_Vx_array.cpp $(build_options)


clean:
	rm test_Vx_array $(gdb_temp_files)

