//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -ggdb3 -o x ./list.cpp


//1.<list>实现简介:
/*
	<list>是最原始的<list 系列>容器, 基于‘自分配内存’来实现.

	list 是一种<链线性>容器, 是一种<链表>的数据结构, 用链表的方式实现,
	擅长频繁插入, 断开等操作;
	也有信息载体记录链表信息, 防止因为求list size()触发<全链遍历>.
	但是不擅长对元素进行排序, 查找.(查找复杂度: O(1-n) 不定)

	list 迭代器:
		由于list 不是连续线性容器, 而是链表,
		所以list 是不能根据下标直接访问元素的.

		//advance(*it,n)//list 迭代器*it向前推进n 位
		注意:
			不能通过it+=1 来取得下一个元素,
			当你需要访问list迭代器后n个元素时,可以用advance();

			'访问下一个元素时', 你可以用next();//++it1;
			'访问上一个元素时', 你可以用prev();//--it2;

附:
	list 不能pop() 空, 否则会访问到空指针.
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



//2.stl_list.h 容器列表:
//实现文件: /usr/include/c++/6.3.0/bits/stl_list.h
/*
	struct _List_node_base, 基础节点
	struct _List_node, (public 继承from _List_node_base), 节点
	struct _List_iterator, 迭代器
	struct _List_const_iterator, 常量迭代器

	class _List_base, 队列信息记录器
	class list, (protected 继承from _List_base), list 的具体实现.
*/



//3.list api list
/*

assign() 给list赋值
back() 返回最后一个元素
begin() 返回指向第一个元素的迭代器
clear() 删除所有元素
empty() 如果list是空的则返回true
end() 返回末尾的迭代器
erase() 删除多个元素
front() 返回第一个元素
get_allocator() 返回list的配置器
insert() 插入一个元素到list中的任意位置
max_size() 返回list能容纳的最大元素数量
merge() 合并两个list
pop_back() 删除最后一个元素
pop_front() 删除第一个元素
push_back() 在list的末尾添加一个元素
push_front() 在list的头部添加一个元素
rbegin() 返回指向第一个元素的逆向迭代器
remove() 从list删除元素
remove_if() 按指定条件删除元素
rend() 指向list末尾的逆向迭代器
resize() 改变list的大小
reverse() 把list的元素倒转
size() 返回list中的元素个数
sort() 给list排序
splice() 合并两个list
swap() 交换两个list
unique() 删除list中重复的元素

*/



#include <stdio.h>
#include <list>
#include <algorithm>//find() 查找函数
using namespace std;



int main(void){
	//1.创建list
	list<int> x1;//											//empty list of ints
	list<int> x2 (4,100);//							//four ints with value 100
	list<int> x3 (x2.begin(),x2.end());//iterating through x2
	list<int> x4 (x3);//								//a copy of x3

	//the iterator constructor can also be used to construct from arrays:
	int arr_tmp[] = {16,2,77,29};
	list<int> x5 (arr_tmp, arr_tmp + sizeof(arr_tmp) / sizeof(int) );
	list<int>::iterator it;//顺向迭代器(比较安全一点的方法)
	list<int>::reverse_iterator rit;//逆向迭代器
	list<int>::iterator it1,it2;//unsigned int != int
	int tmp;
	printf("1 -- \n");



	//2.void clear();//清空list
	x1.clear();
	x3.clear();
	//x1.pop_back();//测试list 是否可以对空list 进行pop()操作.
	//x3.pop_back();//测试结果: 内存溢出! 不能空list 进行pop()操作.
	printf("2 -- \n");



	//3.bool empty();//检查list 是否为空
	if(x1.empty())
		printf("3 -- x1 list is now empty\n");
	if(x2.empty())
		printf("3 -- x2 list is now empty\n");
	if(x3.empty())
		printf("3 -- x3 list is now empty\n");



	//4.size_type size();//返回list 的size()
	printf("4 -- x2 list size()=%d\n", x2.size());



	//向list 插入元素的方式:(方式1) + (方式2)

	//5.push 的方式(方式1)
	//void push_back(const T& x);//向list 尾部压入元素
	x2.push_back(321);

	//void push_front(const T& x);//向list 头部压入元素
	x2.push_front(123);
	printf("5 -- \n");


	//6.用迭代器insert()的方式(方式2)
	it = x2.begin();
	x2.insert(it,555);//从头部插入
	//rit = x2.rend();
	//x2.insert(rit,5555);//不能用反向迭代器操作!!

	it = x2.end();
	x2.insert(it,666);//从尾部插入
	//rit = x2.rbegin();
	//x2.insert(rit,6666);//不能用反向迭代器操作!!
	printf("6 -- \n");



	//7.
	//iterator begin();//返回list的顺向front()元素的迭代器
	//iterator end();//返回list的顺向end()元素的迭代器
	if(!x2.empty()){
		//顺向迭代器遍历(比较安全一点的方法)
		printf("7 -- x2:");
		for(it = x2.begin(); it != x2.end(); *it++)
			printf("%d  ", *it);
		printf("\n");
	}

	//8.
	//iterator rbegin();//返回list的逆向front()元素的迭代器
	//iterator rend();//返回list的逆向end()元素的迭代器
	if(!x2.empty()){
		//逆向迭代器遍历
		printf("8 -- x2:");
		for(rit = x2.rbegin(); rit != x2.rend(); *rit++)
			printf("%d  ", *rit);
		printf("\n");
	}



	//9.list 元素访问典范demo:
	//iterator erase(iterator position);//通过迭代器删除元素
	/*
		//advance(*it,n)//list 迭代器*it向前推进n 位
		注意:
			由于list 不是连续线性容器,
			所以你不能直接通过下标直接访问元素,
			也不能通过it+=1 来取得下一个元素, 必须是'迭代器指针'访问和操作.
		因此:
			当你需要访问list迭代器后n个元素时,可以用advance();
			'访问下一个元素时', 你可以用next();//++it1;
			'访问上一个元素时', 你可以用prev();//--it2;
	*/

	for(tmp=1; tmp<10; tmp++)
		x1.push_back(tmp*10);	//从list 后面插入元素[10-90]
													// 10 20 30 40 50 60 70 80 90
	it1 = it2 = x1.begin(); // ^^
	advance(it2,6);         // ^                 ^
	++it1;                  //    ^              ^

	it1 = x1.erase(it1);    // 10 30 40 50 60 70 80 90
													//    ^           ^

	it2 = x1.erase(it2);    // 10 30 40 50 60 80 90
													//    ^           ^

	++it1;                  //       ^        ^
	--it2;                  //       ^     ^

	x1.erase(it1,it2);      // 10 30 60 80 90
													//       ^^
	printf("9 -- \n");



	//10.弹出元素 && 直接访问头,尾元素
	if(!x2.empty() && x2.size() == 6){
		//reference front();//直接访问第一个元素
		//reference back()//直接访问最后一个元素
		printf("10 -- x2 front=%d, back=%d\n", x2.front(), x2.back());

		//void pop_back();//从尾部弹出元素
		if(!x2.empty())
			x2.pop_back();

		//void pop_front();//从头部弹出元素
		if(!x2.empty())
			x2.pop_front();
		if(!x2.empty())
			x2.pop_front();
		if(!x2.empty())
			x2.pop_front();
		if(!x2.empty())
			x2.pop_front();
		if(!x2.empty())
			x2.pop_front();
	}
	if(x2.empty())
		printf("10 -- x2 list is now empty\n");//元素已经删空
	printf("10 -- x2 list size()=%d\n", x2.size());


	//对空list 错误操作测试
	//x2.pop_front();//list 由于是指针实现的, 绝对不能pop 空一次,
	//x2.pop_front();//否则指针会溢出.
	//x2.pop_front();


	//空队列的x2.front() == NULL == 0 !!
	//注意, 这里不是数值等于0 的意思, 而是空指针的意思
	if(!x2.empty())
		printf("10 -- x2 front=%d,back=%d,空list 头,尾 == NULL!!\n",\
				x2.front(), x2.back());



	//11.void resize(size_type sz, T c = T());
	//	如果容器已经坏了,那么resize()一下,重新修改空deque为0,清空元素.
	x2.resize(0);//重新修改空list 为0.
	printf("11 -- x2 list size()=%d\n", x2.size());
	if(x2.empty())
		printf("11 -- x2 list is now empty\n");

	x2.resize(10,123);//重新创建10个元素, 每个元素都是123
	printf("11 -- x2 list size()=%d\n", x2.size());
	if(x2.empty())
		printf("11 -- x2 list is now empty\n");



	//12.void sort();//对元素进行排序处理
	x2.sort();
	if(!x2.empty()){
		printf("12 -- x2:");
		for(it = x2.begin(); it != x2.end(); *it++)
			printf("%d  ", *it);
		printf("\n");
	}

	printf("12 -- 插入元素后, 再排序\n");
	x2.push_back(11111);//插入元素后, 再排序
	x2.push_back(111);
	x2.push_front(11112);
	x2.sort();
	if(!x2.empty()){
		printf("12 -- x2:");
		for(it = x2.begin(); it != x2.end(); *it++)
			printf("%d  ", *it);
		printf("\n");
	}



	//13.void splice(iterator position,list<T,Allocator>&x);
	//	剪切一个list的所有元素,插入到另外一个List中,完成后清空被剪切的list
	printf("13 -- 剪切一个list的所有元素,插入到另外一个List中.\n");
	printf("13 -- 完成后清空被剪切的list.\n");
	if(!x1.empty()){
		it = x2.begin();
		printf("13 -- x2.begin()=%d\n",*it);

		++it;//x2 points to 2
		x2.splice(it,x1);//将x1所有元素插入x2的第二顺位的元素之间, 并清空x1
		if(x1.empty())
			printf("13 -- x1 list is now empty\n");

		//打印x2
		if(!x2.empty()){
			printf("13 -- x2:");
			for(it = x2.begin(); it != x2.end(); *it++)
				printf("%d  ", *it);
			printf("\n");
		}
	}



	//14.void swap(list<T,Allocator>& lst);//将两个list 的元素互换
	printf("14 -- 将两个list 的元素互换\n");
	if(x1.empty()){
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



	//15.void unique();//去除list 中的'重复值'元素
	printf("15 -- 去除重复元素值测试\n");
	printf("15 -- before: x1 list size()=%d\n", x1.size());
	x1.unique();
	printf("15 -- after: x1 list size()=%d\n", x1.size());

	//打印x1
	if(!x1.empty()){
		printf("15 -- x1:");
		for(it = x1.begin(); it != x1.end(); *it++)
			printf("%d  ", *it);
		printf("\n");
	}



	//16.list 利用std::find()函数做查找.
	//	需要包含算法函数头文件：#include <algorithm>
	//	list API 没有实现find()函数.
	tmp = 123;//查找目标值target
	//it=std::find(x1.begin(),x1.end(),tmp);
	it=find(x1.begin(),x1.end(),tmp);
	if(it != x1.end())
		printf("16 -- it=find(x1.begin(),x1.end(),%d); found it okay!\n",tmp);
	else
		printf("16 -- target-%d cant find\n",tmp);


	return 0;
}



