//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -ggdb3 -o x ./stack.cpp

//1.<优先队列>实现简介:
/*
	基于deque 来实现栈功能, 一种<适配器>容器,
	栈是一种先进后出(FILO)的数据结构.


附: 只要是基于deque实现的容器, 都支持pop()空到负, 成为一个坏容器.
		这只是说明: deque 的实现, 有一定的缓冲区创建为先,
		当缓冲区不够大的时候, 再自动拓展.
		并不能说明, 这种坏容器有实用价值.
		这也对实际使用时, 提出了判别要求, 不要让容器变坏.

		//单个/多个删除erase() or pop() 都行:
		if(x.size() > 0)//等于0 or 小于0 都不让pop()
			x.pop();			//多个删除, 必须用size()

		//单个删除pop()
		if(!x.empty())//这种非0 检测也行, 默认不允许出现'坏容器'
			x.pop();		//只要每个pop() 操作都检查, 也是安全的.

		//查看top() 元素, 也要做判断:
		if(!x.empty())
			x.top();
*/



//2.stack 源码阅读:
/*
	实现文件: /usr/include/c++/6.3.0/bits/stl_stack.h

	stl_stack.h 容器列表:
	class stack,	[基于<deque>实现]stack 的声明与实现.
	ps: 迭代器还是用deque 的迭代器
*/



//3.stack api list
/*

empty() 堆栈为空则返回真
pop() 移除栈顶元素
push() 在栈顶增加元素
size() 返回栈中元素数目
top() 返回栈顶元素

*/



#include <stdio.h>
#include <stack>
#include <string>
#include <vector>
#include <deque>
using namespace std;



int main(void){
	//1.创建栈
	stack<int> test;
	stack<string> test2;//栈不能初始化值
	stack<void*> test3;
	stack<stack<string>> test4;//同样支持异构


	//初始化式, 创建栈:
	//带值初始化,可以将deque or vector里面所有的值,一次写入新生的stack里面
	//但是实际上, stack 内部还是deque 结构
	deque<int> x1(3,100);// x1-deque with 3 elements
	vector<int> x2(2,200);// x2-vector with 2 elements
	stack<int> test5(x1);//如果你想初始化值, 只能这样做.

	//字符串初始化-失败
	//deque<string> x11(3,"fuck");// x11-deque with 3 elements
	//vector<string> x22(2,"you");// x22-vector with 2 elements
	//stack<string> test6(x22);//字符串果然还是不能初始化.

	//数组式-初始化-失败
	//int iarr[16] = {1,2,3,4,5,6,7,8};
	//stack<int> test7(&iarr);
	printf("1 -- \n");



	//***********
	//<整形栈测试>
	//***********
	//2.bool empty();//堆栈为空则返回真
	if(test.empty())
		printf("2 -- 'test stack' is now empty!!\n");//栈空



	//3.void push(const T& x);//在栈顶增加元素
	test.push(1);
	test.push(2);
	test.push(3);
	printf("3 -- \n");



	//4.size_type size();//返回栈中元素数目
	if(test.empty())
		printf("4 -- 'test stack' is still empty!!\n");//栈空
	else
		printf("4 -- 'test stack' size()=%d\n", test.size());



	//5.value_type& top();//返回栈顶元素
	if(!test.empty())//必须先检索堆栈是否为空
		printf("5 -- 'test stack' top=%d\n", test.top());



	//6.void pop(); 移除栈顶元素
	printf("6 -- 'test stack' pop twice\n");
	if(!test.empty())
		test.pop();
	if(!test.empty())
		test.pop();
	if(!test.empty())
		printf("6 -- 'test stack' top=%d\n", test.top());

	printf("6 -- 'test stack' size()=%d\n", test.size());



	//7.最后清空栈后, 看看空栈的时候top() 会显示什么
	printf("7 -- 'test stack' pop third time\n");
	test.pop();
	//test.pop();//如果直接pop() 成负数, empty() 就会失效!!
	//test.pop();
	//test.clear();//不能使用deque() 的函数??不能反向索引

	//仅当size() = 0 的时候, empty() 才会成立!!
	//所以当你pop() 成负数的时候, 整个stack() 就已经是一个坏容器了.
	//所以pop() 之前肯定需要检索是否为empty(),
	//所有的deque 基底的派生容器, 都有这一特性!!
	if(test.empty())
		printf("7 -- 'test stack' is now empty!!\n");

	printf("7 -- 'test stack' size()=%d\n", test.size());

	//ps: 访问栈数据时, 你不知道栈的具体容量, 栈容量是动态变动的,
	//		你必须确保栈不为空才能top(), 否则内存会溢出
	printf("7 -- 'test stack' top=%d\n", test.top());//内存溢出.


	return 0;
}


