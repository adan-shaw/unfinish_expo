//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -ggdb3 -o x ./deque.cpp

//1.<双向队列>实现简介:
/*
<双向队列>是最原始的<队列系列>容器, 基于‘自分配内存’来实现.
deque 是一种<适配器>容器, 一种先进先出(FIFO)的数据结构.


注意: 根据deque.at() 函数的实现:
at(size_type __n){
	_M_range_check(__n);//这个函数只是检查__n >= this->size(),常规溢出检查

	return (*this)[__n];//可以断定,deque 实际就是在数组上面实现的, 
											//所有deque 相关的容器也是有数组属性.
}


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



//2.deque 迭代器:
/*
	struct _Deque_iterator, 实现大部分是内联函数, 这意味着:
	迭代器操作基本都是真实代码操作, 向前向后, 提取目标数据等.
	这些逻辑操作, 实际都是内联到使用者身上的.

	//迭代器实体:
		迭代器一般是用实体, 而不是迭代器-指针;
		实际上, 迭代器的实体, 也是由指针组成的.
		但是具体的指针成分和实现, 就不得而知了, 你也不需要知道太多.
		直接用就行了.

		it 迭代器, ’判别结束条件‘不可以是it == NULL!!
		必须是it != x2.end().
*/



//3.stl_deque.h (源码代码格局):
//实现文件: /usr/include/c++/6.3.0/bits/stl_deque.h
/*
	class _Deque_base, deque 的声明与实现:(源码代码格局)
	[并没有基于<任何容器的基础上>实现, 是'最基础的队列容器']
	deque 容器实现源码, 裸奔, 自己分配内存, 自己管理.

	ps: 以上两个容器的实现, 都是先声明结构体or 类, 后续跟着具体实现.
			例如:
				声明迭代器, 实现迭代器. 声明类, 实现类.
				根据类名(), 你可以很容易找到构造函数和释构函数
*/



//deque api list - stl_deque.h 容器API列表:
/*

assign() 设置双向队列的值
at() 返回指定的元素
back() 返回最后一个元素
begin() 返回指向第一个元素的迭代器
clear() 删除所有元素
empty() 返回真如果双向队列为空
end() 返回指向尾部的迭代器
erase() 删除一个/多个元素
front() 返回第一个元素
get_allocator() 返回双向队列的配置器
insert() 插入一个元素到双向队列中
max_size() 返回双向队列能容纳的最大元素个数
pop_back() 删除尾部的元素
pop_front() 删除头部的元素
push_back() 在尾部加入一个元素
push_front() 在头部加入一个元素
rbegin() 返回指向尾部的逆向迭代器
rend() 返回指向头部的逆向迭代器
resize() 改变双向队列的大小
size() 返回双向队列中元素的个数
swap() 和另一个双向队列交换元素

*/



#include <stdio.h>
#include <deque>
using namespace std;



int main(void){
	//1.创建双向队列 - 多种创建, 集成创建方式.
	deque<int> x1;											//empty deque of ints
	deque<int> x2 (4,100);							//four ints with value 100
	deque<int> x3 (x2.begin(),x2.end());//iterating through x2
	deque<int> x4 (x3);									//a copy of x3
	//the iterator constructor can also be used to construct from arrays:
	int myints[] = {16,2,77,29};
	deque<int> x5 (myints, myints + sizeof(myints) / sizeof(int) );

	int tmp,size;
	deque<int>::iterator it;//顺向迭代器
	deque<int>::reverse_iterator rit;//逆向迭代器
	printf("1 -- \n");



	//2.void clear();//清空双向队列
	x1.clear();
	x3.clear();
	printf("2 -- \n");



	//3.bool empty();//检查双向队列是否为空
	if(x1.empty())
		printf("3 -- x1 deque is now empty\n");
	if(x2.empty())
		printf("3 -- x2 deque is now empty\n");
	if(x3.empty())
		printf("3 -- x3 deque is now empty\n");



	//4.size_type size();//返回deque 的size()
	printf("4 -- x2 deque size()=%d\n", x2.size());



	//5.void push_back(const T& x);//向deque 尾部压入元素
	x2.push_back(321);
	printf("5 -- \n");

	//6.void push_front(const T& x);//向deque 头部压入元素
	x2.push_front(123);
	printf("6 -- \n");



	//7.reference at(size_type n);//下表访问元素
	if(!x2.empty()){
		//第一种遍历元素的方法: at() 遍历
		printf("7 -- x2:");
		for(tmp=0,size=x2.size();tmp < size;tmp++)
			printf("at(%d)=%d,", tmp, x2.at(tmp));
		printf("\n");
	}
	if(!x2.empty()){
		//直接下标访问
		printf("7 -- x2:");
		for(tmp = 0;tmp < size;tmp++)
			printf("x2[%d]=%d,", tmp, x2[tmp]);
		printf("\n");
	}



	//8.
	//iterator begin();//返回双向队列--顺向front() 元素的迭代器
	//iterator end();//返回双向队列--顺向end() 元素的迭代器
	if(!x2.empty()){
		//顺向迭代器遍历(比较安全一点的方法)
		printf("8 -- x2:");
		for(it = x2.begin(); it != x2.end(); *it++)
			printf("%d  ", *it);
		printf("\n");
	}

	//9.
	//iterator rbegin();//返回双向队列--逆向front() 元素的迭代器
	//iterator rend();//返回双向队列--逆向end() 元素的迭代器
	if(!x2.empty()){
		//逆向迭代器遍历
		printf("9 -- x2:");
		for(rit = x2.rbegin(); rit != x2.rend(); *rit++)
			printf("%d  ", *rit);
		printf("\n");
	}



	//10.deque 迭代器位移
	//iterator erase(iterator position);//通过迭代器删除元素
	if(!x2.empty() && x2.size() == 6){
		//***************
		for(tmp=0,it=x2.begin();tmp<2;tmp++){//移除begin()头部的两个元素
			//x2.erase(*it+=1);//错误写法
			if(!x2.empty())
				x2.erase(it+=1);//等价于*++it 移动迭代器
												//*it++ 是操作完之后再加1
		}
		//***************

		//再删除2 个元素. 这次是迭代器段删除
		if(x2.size() >=2 )//多个删除, 必须用size()
			x2.erase(x2.begin()+1,x2.begin()+3);

		//遍历打印剩余的123,321
		printf("10 -- x2:");
		for(it = x2.begin(); it != x2.end(); *it++)
			printf("%d,", *it);
		printf("\n");
	}



	//11.弹出元素 && 直接访问头,尾元素


	//void pop_back();//从尾部弹出元素
	if(!x2.empty())
		x2.pop_back();

	//reference front();//直接访问第一个元素
	//reference back()//直接访问最后一个元素
	if(!x2.empty())
		printf("11 -- x2 front=%d, back=%d, 只有一个元素时, 头等于尾.\n",\
				x2.front(), x2.back());

	//void pop_front();//从头部弹出元素
	if(!x2.empty())
		x2.pop_front();
	if(x2.empty())
		printf("11 -- x2 deque is now empty\n");//元素已经删空


	//12.对空deque 错误弹出操作测试
	//注意:
	/*
		pop_front()只对头操作, 可以pop 成负数;
		所以任何pop 操作都要检索deque 是否为empty();
		否则deque 可能会被pop 坏;
		deque 内部计数出现严重错误, 影响后续运作.
	 */
	x2.pop_front();
	x2.pop_front();
	x2.pop_front();
	printf("12 -- x2 front=%d, back=%d, 对空deque错误弹出\n",\
			x2.front(), x2.back());
	if(x2.empty())
		printf("12 -- x2 deque is now empty\n");
	else
		printf("12 -- x2 deque size()=%d\n", x2.size());



	//13.void resize(size_type sz, T c = T());
	//	如果容器已经坏了,那么resize()一下,重新修改空deque为0,清空元素.
	x2.resize(0);
	printf("13 -- x2 deque size()=%d\n", x2.size());
	if(x2.empty())
		printf("13 -- x2 deque is now empty\n");

	//重新创建10个元素, 每个元素都是123
	x2.resize(10,123);
	printf("13 -- x2.resize(10,123)\n");
	printf("13 -- x2 deque size()=%d\n", x2.size());
	if(x2.empty())
		printf("13 -- x2 deque is now empty\n");



	//14.void swap(list<T,Allocator>& dqe);//将两个deque 的元素互换
	if(x1.empty()){
		printf("14 -- 将两个deque 的元素互换\n");
		x1.push_front(8888);//x1 此时为空, 插入一个元素,
		x1.swap(x2);//将x1 x2的所有元素互相交换


		//打印x1
		if(!x1.empty()){
			printf("14 -- x1:");
			for(it = x1.begin(); it != x1.end(); *it++)
				printf("%d  ", *it);
			printf("\n");
		}

		//打印x2
		if(!x2.empty()){
			printf("14 -- x2:");
			for(it = x2.begin(); it != x2.end(); *it++)
				printf("%d  ", *it);
			printf("\n");
		}
	}


	return 0;
}


