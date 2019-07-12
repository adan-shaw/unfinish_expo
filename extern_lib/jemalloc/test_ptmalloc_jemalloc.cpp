//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -ggdb3 -o x ./test_ptmalloc_jemalloc.cpp \
-Bstatic -DJEMALLOC_NO_DEMANGLE

//本例子, 说明ptmalloc 和jemalloc 并存的可能性.
//但是实际应用没有必要, 只需要一种内存管理器.



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jemalloc/jemalloc.h>
#include <errno.h>



//声明jemalloc 函数(此时malloc() 并没有被替换)
#define MALLOC je_malloc
#define FREE je_free
#define REALLOC je_realloc

#define test_malloc_max 64



int main(void){
	char* p;



	//1.jemalloc--宏定义je_malloc()为MALLOC(), 方便区别
	p=(char*)MALLOC(test_malloc_max);//分配64 bytes 字节内存
	memset(p,'\0',test_malloc_max);
	strncpy(p,"fuck you bitch\n",test_malloc_max);
	fprintf(stderr, "%s", p);
	FREE(p);


	//2.ptmalloc--保持原来的接口
	p=NULL;
	p=(char*)malloc(test_malloc_max);
	memset(p,'\0',test_malloc_max);
	strncpy(p,"fuck you bitch\n",test_malloc_max);
	fprintf(stderr, "%s", p);
	free(p);

	return 0;
}
