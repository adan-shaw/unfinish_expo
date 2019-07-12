//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -ggdb3 -o x ./priority_queue.cpp

//1.<优先队列>实现简介:
/*
	priority_queue 是一种<适配器>容器,
	一种<带优先级操作>的先进先出(FIFO)的数据结构.

	会自动根据int 的数值大小排序!!
	数值大的在top() !! 而且只能读出top().

	priority_queue 基于deque队列的基础上实现,
	大部分特性集成于deque, 实际就是对deque 的"剪裁输出".


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



//2.priority_queue 源码阅读:
/*
	实现文件: /usr/include/c++/6.3.0/bits/priority_queue.h
	priority_queue.h 容器列表:
	class queue,					[基于<deque>实现]queue 的声明与实现.
	class priority_queue, [基于<deque>实现]priority_queue的声明与实现.
	ps: 迭代器还是用deque 的迭代器
*/



//3.priority_queue api list
/*

empty() 如果优先队列为空，则返回真
pop() 从top顶部, 删除一个元素
push() 从foot底部, 加入一个元素
size() 返回'优先队列'中拥有的元素的个数
top() 返回'优先队列'中有最高优先级的‘顶部top元素’

*/



#include <stdio.h>
#include <queue>
using namespace std;



int main(void){
	//1.创建’优先队列‘
	priority_queue<int> x1;
	int arr_tmp[]= {10,60,50,20};
	priority_queue<int> x2(arr_tmp,arr_tmp+3);
	printf("1 -- \n");



	//2.size_type size();//返回‘优先队列’size()
	printf("2 -- x1 priority_queue size()=%d\n",x1.size());
	printf("2 -- x2 priority_queue size()=%d\n",x2.size());



	//3.bool empty();//判断‘优先队列’是否为空
	if(x1.empty())
		printf("3 -- x1 priority_queue is now empty\n");
	if(x2.empty())
		printf("3 -- x2 priority_queue is now empty\n");



	//4.const value_type& top()const;//返回‘优先队列’top元素的值
	if(!x2.empty())
		printf("4 -- x2 priority_queue top()=%d\n", x2.top());



	//5.void pop();//弹出‘优先队列’top元素的值
	if(!x2.empty())
		printf("5 -- x2 priority_queue top()=%d\n", x2.top());
	if(!x2.empty())
		x2.pop();//弹出top 值
	if(!x2.empty())
		printf("5 -- x2 priority_queue top()=%d\n", x2.top());



	//6.void push(const T& x);//从foor底部压入一个值
	x2.push(1111);//从foor压入一个‘新的最大值’,
								//顺便测试一下priority_queue 自动排序的功能.
								//priority_queue会根据数值大小, 自动排序,
								//大的值在top顶部.
	if(!x2.empty())
		printf("5 -- x2 priority_queue top()=%d\n", x2.top());



	//6.清空优先队列 && 溢出测试
	if(!x2.empty())
		x2.pop();
	if(!x2.empty())
		x2.pop();
	if(!x2.empty())
		x2.pop();
	printf("6 -- x2 priority_queue size()=%d\n", x2.size());
	if(x2.empty())
		printf("6 -- x2 priority_queue is now empty\n");


	//未初始化的<优先队列top> 为空指针, 不能操作, 否则内存溢出
	//printf("6 -- x1 priority_queue top()=%d\n", x1.top());


	//溢出测试
	x2.pop();
	x2.pop();//超出pop() 测试 okay
	if(x2.empty())
		printf("6 -- x2 priority_queue is now empty\n");

	//溢出仍能工作, 但是没啥意义.
	printf("6 -- x2 priority_queue size()=%d\n", x2.size());
	printf("6 -- x2 priority_queue top()=%d\n", x2.top());


	return 0;
}


