//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -ggdb3 -o x ./vector.cpp


//1.<向量>实现简介:
/*
	最原始的<向量系列>容器, 基于自分配内存来实现.
	vector 是一种<数组>容器, 是一种<数组自动增长>的数据结构.

	VECTOR的工作原理是系统预先分配一块CAPACITY大小的空间,
	当插入的数据超过这个空间的时候, 这块空间会让某种方式扩展,
	但是你删除数据的时候, 它却不会缩小.
	vector为了防止大量分配连续内存的开销,
	保持一块默认的尺寸的内存, clear只是清数据了
	未清内存, 因为vector的capacity容量未变化, 系统维护一个的默认值

	所以其实向量也很好用.比你自己造的容器要安全, 性能也不会低.

附:
	vector 不能pop() 空, 否则会访问到空指针.
	所以pop(), 迭代器遍历, front() back() 等访问操作, 都要做非空检测!
	erase() 迭代器多个删除, 必须用if(size() > n) 判断是否会删除溢出.

	//迭代器实体:
		迭代器一般是用实体, 而不是迭代器-指针;
		实际上, 迭代器的实体, 也是由指针组成的.
		但是具体的指针成分和实现, 就不得而知了, 你也不需要知道太多.
		直接用就行了.

		it 迭代器, ’判别结束条件‘不可以是it == NULL!!
		必须是it != x2.end().
*/



//2.stack 源码阅读:
/*
	实现文件: /usr/include/c++/6.3.0/bits/stl_vector.h
	stl_vector.h 容器列表:
	struct _Vector_base,	向量的信息记录体
	class vector,					vector 的声明与实现.
												[并没有基于<任何容器的基础上>实现]
												裸奔, 自己分配内存, 自己管理.
*/



//3.vector api list
/*

assign() 对Vector中的元素赋值
at() 返回指定位置的元素
back() 返回最末一个元素
begin() 返回第一个元素的迭代器
capacity() 返回vector所能容纳的元素数量(在不重新分配内存的情况下）
clear() 清空所有元素
empty() 判断Vector是否为空（返回true时为空）
end() 返回最末元素的迭代器(译注:实指向最末元素的下一个位置)
erase() 删除指定元素
front() 返回第一个元素
get_allocator() 返回vector的内存分配器
insert() 插入元素到Vector中
max_size() 返回Vector所能容纳元素的最大数量（上限）
pop_back() 移除最后一个元素
push_back() 在Vector最后添加一个元素
rbegin() 返回Vector尾部的逆迭代器
rend() 返回Vector起始的逆迭代器
reserve() 设置Vector最小的元素容纳数量
resize() 改变Vector元素数量的大小
size() 返回Vector元素数量的大小
swap() 交换两个Vector

*/



#include <stdio.h>
#include <vector>
using namespace std;



int main(void){
	//1.创建空的向量
	vector<int> x1;											// empty vector of ints
	vector<int> x2(4,100);							// four ints with value 100
	vector<int> x3(x2.begin(),x2.end());// iterating through x2
	vector<int> x4(x3);									// a copy of x3
	int tmp,size;
	vector<int>::iterator it;//顺向迭代器(比较安全一点的方法)
	vector<int>::reverse_iterator rit;//逆向迭代器
	printf("1 -- \n");



	//2.void clear();//清空向量
	x1.clear();
	x3.clear();
	printf("2 -- \n");



	//3.bool empty();//判断向量是否为空
	if(x1.empty())
		printf("3 -- x1 vector is now empty\n");
	if(x2.empty())
		printf("3 -- x2 vector is now empty\n");
	if(x3.empty())
		printf("3 -- x3 vector is now empty\n");



	//4.size_type size();//返回vector 的size()
	printf("4 -- x2 vector size()=%d\n", x2.size());



	//5.void push_back(const T& x);//向vector 尾部压入元素
	x2.push_back(321);
	x2.push_back(123);
	printf("5 -- \n");



	//6.遍历方式1
	//reference at(size_type n);下表访问元素
	if(!x2.empty()){
		//第一种遍历元素的方法: at() 遍历
		printf("6 -- x2:");
		for(tmp=0,size = x2.size();tmp < size;tmp++)
			printf("at(%d)=%d  ", tmp, x2.at(tmp));
		printf("\n");

		//第二种遍历元素的方法: 直接下标访问
		printf("6 -- x2:");
		for(tmp = 0;tmp < size;tmp++)
			printf("x2[%d] = %d  ", tmp, x2[tmp]);
		printf("\n");
	}



	//7.顺向迭代器
	//iterator begin();//返回向量--顺向front() 元素的迭代器
	//iterator end();//返回向量--顺向end() 元素的迭代器
	if(!x2.empty()){
		printf("7 -- x2:");
		for(it = x2.begin(); it != x2.end(); *it++)
			printf("%d  ", *it);
		printf("\n");
	}



	//8.逆向迭代器
	//iterator rbegin();//返回向量--逆向front() 元素的迭代器
	//iterator rend();//返回向量--逆向end() 元素的迭代器
	if(!x2.empty()){
		printf("8 -- x2:");
		for(rit = x2.rbegin(); rit != x2.rend(); *rit++)
			printf("%d  ", *rit);
		printf("\n");
	}



	//9.iterator erase(iterator position);//通过迭代器删除元素
	if(!x2.empty() && x2.size() == 6){
		//***************
		it = x2.begin();
		for(tmp=1;tmp<3;tmp++){//移除begin() 下一个元素, 两次
			//x2.erase(*it+=1);//错误写法
			if(!x2.empty())
				x2.erase(it+=1);//等价于*++it 移动迭代器
												//*it++ 是操作完之后再加1
		}
		//***************

		//再删除2 个元素. 这次是迭代器段删除
		if(x2.size() >=2 )//多个删除, 必须用size()
			x2.erase(x2.begin()+1,x2.begin()+3);

		//打印剩余的123,321
		if(!x2.empty()){
			printf("9 -- x2:");
			for(it = x2.begin(); it != x2.end(); *it++)
				printf("%d  ", *it);
			printf("\n");
		}
	}



	//10.void pop_back();//从尾部弹出元素
	if(!x2.empty())
		x2.pop_back();

	//void pop_front();	//从头部弹出元素
	if(!x2.empty())
		x2.pop_back();
	printf("10 -- \n");



	//11.
	//reference front();//返回front 元素
	//reference back();//返回back 元素
	if(!x2.empty())
		printf("11 -- x2 front=%d, back=%d, 只有一个元素时, 头等于尾.\n",\
				x2.front(), x2.back());
	if(!x2.empty())
		x2.pop_back();

	if(x2.empty())
		printf("11 -- x2 vector is now empty\n");//元素已经删空



	//12.对空vector 错误操作测试
	x2.pop_back();//pop_back 只对尾操作, 可以pop 成负数
	x2.pop_back();//所以任何pop 操作都要检索vector 是否为empty()
	x2.pop_back();//否则vector 可能会被pop 坏
	printf("12 -- x2 front=%d, back=%d, 对空deque错误弹出\n",\
			x2.front(), x2.back());
	if(x2.empty())
		printf("12 -- x2 deque is now empty\n");
	else
		printf("12 -- x2 deque size()=%d\n", x2.size());



	//13.void resize(size_type sz, T c = T());
	//	如果容器已经坏了,那么resize()一下,重新修改空deque为0,清空元素.
	x2.resize(0);
	printf("13 -- x2 vector size()=%d\n", x2.size());
	if(x2.empty())
		printf("13 -- x2 vector is now empty\n");


	x2.resize(10,123);//重新创建10个元素, 每个元素都是123
	printf("13 -- x2 vector size()=%d\n", x2.size());
	if(x2.empty())
		printf("13 -- x2 vector is now empty\n");



	//14.void swap(vector<T,Allocator>& vec);//将两个stack 的元素互换
	if(x1.empty()){
		printf("14 -- 将两个stack 的元素互换\n");
		x1.push_back(8888);//x1 此时为空, 插入一个元素,
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


