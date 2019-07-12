//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,

//编译:
//g++ -ggdb3 -o x ./jemalloc_static.cpp \
-Bstatic -DJEMALLOC_NO_DEMANGLE



//jemalloc使用方式:
/*
1 查看有没有使用jemalloc
	可以直接用ldd来看看有没有关联到jemlloc 的共享库即可.
	有则有使用jemalloc.
		ldd ./x
	注意:
		如果是动态链接, ldd ./x 会显示关联链接库为xxx.so 动态链接库
		如果是静态链接, ldd ./x 会显示关联链接库为xxx.a 静态链接库


2 jemalloc 动静编译的区别:
	jemmalloc 是动态链接库编译, 还是静态链接库编译, 全看编译选项而已.
	只要正确安装jemalloc 之后,
	动态链接编译选项:
		-Bdynamic -ljemalloc
	静态链接编译选项:
		-Bstatic

	最关键的编译选项: -DJEMALLOC_NO_DEMANGLE
	这个编译选项表示: 是否荣je_malloc() je_free() 替换malloc() free(),

	如果替换, 则ptmalloc 会彻底失效, 被jemalloc 取代.

	如果不替换, malloc() free() 还是ptmalloc,
	只有je_malloc() je_free() 才是jemalloc.
	两种内存管理器并存.(不推荐!!)


	ps: 编译选项: -Bdynamic [尽可能使用动态库]
			静态tarball, 可以不make install, 但是需要指明.h文件 + .a文件
			编译选项: -Bstatic	[尽可能使用静态库编译]
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jemalloc/jemalloc.h>



#define test_malloc_max 64



int main(void){
	char*p = (char*)malloc(test_malloc_max);//分配64 bytes 字节内存



	memset(p,'\0',test_malloc_max);
	strncpy(p,"fuck you bitch\n",test_malloc_max);
	fprintf(stderr, "%s", p);
	free(p);

	return 0;
}
