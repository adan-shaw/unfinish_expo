//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -ggdb3 -o x ./multi_set.cpp

//1.<多元集合>实现简介:
/*
	基于红黑树实现的multi_set 多元集合功能, 是一种有序关联容器.
	多元集合(multiset)和集合(set)相像, 只不过支持重复对象.
	提供快速查找, 较快的增删速度(查找复杂度: O(log n) )

	typedef _Rb_tree<key_type, value_type, _Identity<value_type>,
						key_compare, _Key_alloc_type> _Rep_type;//声明红黑树的别名
	_Rep_type _M_t;// Red-black tree representing multi_set.
	这两句表明multi_set 是红黑树实现的, 迭代器也是红黑树迭代器.
	这两句声明了一个红黑树容器, 并且将其<别名化: _Rep_type>
*/



//2.源码简释
/*
	实现文件: /usr/include/c++/6.3.0/bits/multiset.h
	multiset.h 容器列表:
	class multiset,		 multiset 的声明与实现.
*/



//3.multi_set apt list
/*

begin() 返回指向第一个元素的迭代器
clear() 清除所有元素
count() 返回指向某个值元素的个数
empty() 如果集合为空, 返回true
end() 返回指向最后一个元素的迭代器
equal_range() 返回集合中与给定值相等的上下限的两个迭代器
erase() 删除集合中的元素
find() 返回一个指向被查找到元素的迭代器
get_allocator() 返回多元集合的分配器
insert() 在集合中插入元素
key_comp() 返回一个用于元素间值比较的函数
lower_bound() 返回指向大于（或等于）某值的第一个元素的迭代器
max_size() 返回集合能容纳的元素的最大限值
rbegin() 返回指向多元集合中最后一个元素的反向迭代器
rend() 返回指向多元集合中第一个元素的反向迭代器
size() 多元集合中元素的数目
swap() 交换两个多元集合变量
upper_bound() 返回一个大于某个值元素的迭代器
value_comp() 返回一个用于比较元素间的值的函数

*/



#include <stdio.h>
#include <string>
#include <set>//multi set 一样声明在set 中
using namespace std;



//使用'自定义结构'的multiset
/*
不过'自定义结构'是没有任何用处的,
因为multiset并不知道如何去比较一个自定义的类型.
所有的multiset 原有的API 里面, 很多原有的函数都会失效,
这样的'自定义结构体'容器, 使用意义不大.

重写大部分api 是傻逼的做法,
要是需要插入复杂结构, 可以考虑multimap,
用int 做索引=key, 用指针作为value, 存放复杂结构的数据体.
 */
struct rec{
	int x,y;
};
multiset<rec>h;




int main(void){
	int x;
	multiset<int>h;//建立一个multiset类型, 变量名是h,
								 //h序列里面存的是int类型,初始h为空
	printf("输入0 结束scanf(), 允许有重复值的set !!\n");
	printf("但int 绝对不允许有字符!! 否则会卡死, 发生错误!!\n");



	scanf("%ld",&x);
	while(x!=0){
		h.insert(x);        //将x插入h中
		scanf("%ld",&x);
	}
	while(!h.empty()){    //序列非空 h.empty()==true时 表示h已经空了
		__typeof(h.begin()) c=h.begin();
												//c指向h序列中第一个元素的地址, 第一个元素是最小的元素
		printf("%ld ",*c);  //将地址c存的数据输出
		h.erase(c);         //从h序列中将c指向的元素删除
	}


	return 0;
}


