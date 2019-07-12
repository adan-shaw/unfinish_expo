//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


弄玩这个map,,c++ stl 容器部分先歇一歇～～～做点别的了,,,
多键容器下次再拓展, 也是很重要的,,, 但是也是最后两个多键容器了～～


//编译:
//g++ -ggdb3 -o x ./multi_map.cpp

//1.<多元map>实现简介:
/*
	基于红黑树实现的multi_map 多元map 功能, 是一种有序关联容器.
	multimap 和map 很相似, 但是multimap允许重复的元素.
	提供快速查找, 较快的增删速度(查找复杂度: O(log n) )

	typedef _Rb_tree<key_type, value_type, _Identity<value_type>,
					key_compare, _Key_alloc_type> _Rep_type;//声明红黑树的别名
	_Rep_type _M_t;// Red-black tree representing multi_map.
	这两句表明multi_map 是红黑树实现的, 迭代器也是红黑树迭代器.
	这两句声明了一个红黑树容器, 并且将其<别名化: _Rep_type>
*/



//2.源码简释
/*
	实现文件: /usr/include/c++/6.3.0/bits/multimap.h
	multimap.h 容器列表:
	class multimap,		 multimap 的声明与实现.
*/



//3.multi_map api list
/*

begin() 返回指向第一个元素的迭代器
clear() 删除所有元素
count() 返回一个元素出现的次数
empty() 如果multimap为空则返回真
end() 返回一个指向multimap末尾的迭代器
equal_range() 返回指向元素的key为指定值的迭代器对
erase() 删除元素
find() 查找元素
get_allocator() 返回multimap的配置器
insert() 插入元素
key_comp() 返回比较key的函数
lower_bound() 返回键值>=给定元素的第一个位置
max_size() 返回可以容纳的最大元素个数
rbegin() 返回一个指向mulitmap尾部的逆向迭代器
rend() 返回一个指向multimap头部的逆向迭代器
size() 返回multimap中元素的个数
swap() 交换两个multimaps
upper_bound() 返回键值>给定元素的第一个位置
value_comp() 返回比较元素value的函数

*/



#include <stdio.h>
#include <iostream>
#include <string>
#include <map>//multi map 一样声明在map 中
using namespace std;



using Pet_type = string;
using Pet_name = string;



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
multimap<int,rec>h;//这样就能很好地避免'复杂结构'影响容器的运作.
									 //是比较常用的方法



int main(){
	multimap<string,int> m_map;
	string s("中国"),s1("美国");
	int k;
	multimap<string,int>::iterator m;
	multimap<string,int>::iterator beg,end;



	m_map.insert(make_pair(s,50));
	m_map.insert(make_pair(s,55));
	m_map.insert(make_pair(s,60));
	m_map.insert(make_pair(s1,30));
	m_map.insert(make_pair(s1,20));
	m_map.insert(make_pair(s1,10));



	//方式1
	m = m_map.find(s);
	for(k = 0;k != m_map.count(s);k++,m++)
			cout<<m->first<<"--"<<m->second<<endl;

	//方式2
	beg = m_map.lower_bound(s1);
	end = m_map.upper_bound(s1);
	for(m = beg;m != end;m++)
			cout<<m->first<<"--"<<m->second<<endl;

	//方式3
	beg = m_map.equal_range(s).first;
	end = m_map.equal_range(s).second;
	for(m = beg;m != end;m++)
			cout<<m->first<<"--"<<m->second<<endl;


	return 0;
}



