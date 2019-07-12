//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -ggdb3 -o x ./map.cpp -w

//1.<map>实现简介:
/*
	基于红黑树实现的map 集合功能, 是一种有序关联容器.
	提供快速查找, 较快的增删速度(查找复杂度: O(log n) )

	typedef _Rb_tree<key_type, value_type, _Identity<value_type>,
						key_compare, _Key_alloc_type> _Rep_type;//声明红黑树的别名
	_Rep_type _M_t;//The actual tree structure.
	这两句表明map 是红黑树实现的, 迭代器也是红黑树迭代器.
	这两句声明了一个红黑树容器, 并且将其<别名化: _Rep_type>

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



//2.源码简释
/*
	实现文件: /usr/include/c++/6.3.0/bits/stl_map.h
	stl_map.h容器列表:
	class map,		map 的声明与实现.
*/



//3.map api list
/*

begin() 返回指向map头部的迭代器
clear() 删除所有元素
count() 返回指定元素出现的次数
empty() 如果map为空则返回true
end() 返回指向map末尾的迭代器
equal_range() 返回特殊条目的迭代器对
erase() 删除一个元素
find() 查找一个元素
get_allocator() 返回map的配置器
insert() 插入元素
key_comp() 返回比较元素key的函数
lower_bound() 返回键值>=给定元素的第一个位置
max_size() 返回可以容纳的最大元素个数
rbegin() 返回一个指向map尾部的逆向迭代器
rend() 返回一个指向map头部的逆向迭代器
size() 返回map中元素的个数
swap() 交换两个map
upper_bound() 返回键值>给定元素的第一个位置
value_comp() 返回比较元素value的函数

*/



#include <iostream>
#include <stdio.h>
#include <map>
using namespace std;



int main(void){
	//1.创建集合
	map<int,int> x1;//empty map of ints
	map<int,int> x2;//pointers used as iterators

	//迭代器实体:
	/*
		迭代器一般是用实体, 而不是迭代器-指针;
		实际上, 迭代器的实体, 也是由指针组成的.
		但是具体的指针成分和实现, 就不得而知了, 你也不需要知道太多.
		直接用就行了.
	*/
	map<int,int>::iterator it;//顺向迭代器(比较安全一点的方法)
	map<int,int>::reverse_iterator rit;//逆向迭代器
	map<int,int> mymap;
	map<int,int>::iterator itlow,itup;

	map<int,int>::key_compare kcomp;//取出key-value 的key 集


	//x2 初始化, 插入map 元素!!
	x2.insert(pair<int,int>(0,10));//插入方式1: pair() + insert()
	x2.insert(pair<int,int>(1,20));
	x2.insert(pair<int,int>(2,30));
	x2.insert(pair<int,int>(3,40));
	x2.insert(pair<int,int>(4,50));

	//插入方式2: 数组插入方式, 仅当map key=int 的时候有效.
	x2[5] = 60;
	x2[6] = 70;

	//插入方式3:value_type() + insert()
	x2.insert(map<int, int>::value_type(7, 80));
	x2.insert(map<int, int>::value_type(8, 90));


	//失败, 并不能重新调用'构造函数'. 构造函数只能在创建变量的时候调用.
	/*
		x3(x2);//重新调用构造函数, 拷贝x2 map 里面的所有值
		x4(x2.begin(), x2.end());//重新调用构造函数, 拷贝x2 map 里面的所有值
	*/
	map<int,int> x3(x2);
	map<int,int> x4(x2.begin(), x2.end());
	//要拷贝map, 建议还是做个遍历比较好;
	//不同类型的map 集合,拷贝遍历函数都不一样,这里以<int,int>类型为例:
	//(详细可以参考不同的插入方式)
	for(it = x2.begin();it != x2.end();it++)
		x3[it->first] = it->second;


	//set集合里面的, 用数组初始化set集合的方式, 在map里面是禁止的!
	/*
	int myints[]= {10,20,30,40,50};
	set<int> x2 (myints,myints+5);//pointers used as iterators
	*/

	int tmp;
	printf("1 -- \n");



	//2.void clear();//清空map
	x1.clear();
	x3.clear();
	printf("2 -- \n");



	//3.bool empty();//检查map 是否为空
	if(x1.empty())
		printf("3 -- x1 map is now empty\n");
	if(x2.empty())
		printf("3 -- x2 map is now empty\n");
	if(x3.empty())
		printf("3 -- x3 map is now empty\n");



	//4.size_type size();//返回map 的size()
	printf("4 -- x2 map size()=%d\n", x2.size());



	//5.顺向迭代器遍历
	//iterator begin();//返回map--顺向front() 元素的迭代器
	//iterator end();//返回map--顺向end() 元素的迭代器
	if(!x2.empty()){
		//顺向迭代器遍历(比较安全一点的方法)
		printf("5 -- x2:");
		for(it = x2.begin(); it != x2.end(); *it++)
			printf("%d-%d  ", (*it).first,(*it).second);
		printf("\n");
	}

	//6.逆向迭代器遍历
	//iterator rbegin();//返回map--逆向front() 元素的迭代器
	//iterator rend();//返回map--逆向end() 元素的迭代器
	if(!x2.empty()){
		//逆向迭代器遍历
		printf("6 -- x2:");
		for(rit = x2.rbegin(); rit != x2.rend(); *rit++)
			printf("%d-%d  ", *rit, (*rit).second);
		printf("\n");
	}



	//7.删除元素--三种删除方式
	//void erase(iterator position);//单个迭代器删除
	//size_type erase(const key_type& x);//常量值输入删除
																			 //值不存在就不删除,也不会失败
	//void erase(iterator first, iterator last);//迭代器区域删除
	if(!x2.empty()){
		it=x2.begin();
		printf("7 -- erase first: key=%d,val=%d\n",*it,(*it).second);
		x2.erase(it);//删除第一个值(单个迭代器删除)
	}

	if(!x2.empty())
		x2.erase(20);//(常量值输入删除)

	if(!x2.empty()){
		rit=x2.rbegin();
		printf("7 -- last element value is:%d\n",*rit);
	}

	//迭代器区域删除
	it=x2.find(30);
	if(it != x2.end()){
		printf("7 -- erase <%d-%d>\n",*it,*x2.end());
		x2.erase(it, x2.end());//删除30 后面的(也就是删除少于30 的值)
													 //注意map 是自动排序的, 从小到大排序
	}


	//打印删除后的剩下元素--应该剩下一个20
	if(!x2.empty()){
		//顺向迭代器遍历(比较安全一点的方法)
		printf("7 -- x2:");
		for(it = x2.begin(); it != x2.end(); *it++)
			printf("%d-%d  ", (*it).first,(*it).second);
		printf("\n");
	}



	//8.插入新元素
	//千万不要用这样的数字表示, 如: 023, 0203
	//这表示不是十进制的数字, 麻烦
	it=x2.begin();
	x2.insert (it, pair<int,int>(9,123));//插入方式1
	x2.insert (it, pair<int,int>(9,65));
	x2.insert (it, pair<int,int>(11,3));
	x2.insert (it, pair<int,int>(91,21));

	x2[101] = 101;//插入方式2 -- 仅当key = int 时有效
	x2[102] = 45632;

	//插入方式3:value_type() + insert()
	x2.insert(map<int, int>::value_type(70, 800));
	x2.insert(map<int, int>::value_type(80, 900));
	printf("8 -- \n");



	//9.void swap(map<Key,Compare,Allocator>& st);//map 之间交换元素
	x1.swap(x2);

	//打印交换后的结果
	if(!x1.empty()){
		printf("9 -- x1: ");
		for(rit = x1.rbegin(); rit != x1.rend(); *rit++)
			cout << (*rit).first << "-"<< (*rit).second << "  ";
		cout << endl;
	}

	if(!x2.empty()){
		printf("9 -- x2: ");
		for(rit = x2.rbegin(); rit != x2.rend(); *rit++)
			cout << (*rit).first << "-"<< (*rit).second << "  ";
		cout << endl;
	}
	else
		printf("9 -- x2: empty!!\n");



	//*********
	//查找类方法
	//*********
	//10.size_type count(cont key_type& x);//统计某个值元素的个数
	printf("10 -- x1, element '20' count=%d\n",x1.count(20));
	printf("10 -- x1, element '11' count=%d\n",x1.count(11));



	//11.iterator find(const key_type& x);//查找指定元素
	it=x1.find(11);
	if(it != x1.end())//it 迭代器, ’判别结束条件‘不可以是it==NULL!!
										//必须是it != x2.end()
		printf("11 -- x1.find(11) = %d\n", *it);
	else
		printf("11 -- x1.find(11) fail! no element<11>\n", *it);



	//************************************************************
	//查找区域内的元素值, 暂时弃用, 不知道用途
	//pair<iterator,iterator> equal_range ( const key_type& x ) const;
	//pair是将2个数据组合成一个数据, 当需要这样的需求时就可以使用pair,
	//如stl中的map就是将key和value放在一起来保存...
	//但是这里, pair->first = lower_bound, pair->second = upper_bound

	//所以?? 在map 里面, pair 总是指向两个连续元素？？
	//pair<map<int,int>::iterator,map<int,int>::iterator> ret;
	//ret = x1.equal_range(20);//不知道有什么用.
	//printf("%d--%d\n", *ret.first,*ret.second);
	//printf("\n\n");
	//************************************************************



	//12.iterator lower_bound(const key_type& x);
	//返回指向首个不小于<给定键>的元素的迭代器 -- <给键>不一定要存在于map中
	//所有key不小于 n 的元素, 但是等于也算是不小于
	it = x1.lower_bound(2);
	if(it != x1.end())
		printf("12 -- lower_bound(2) = %d-%d\n", it->first,it->second);


	//13.iterator upper_bound(const key_type& x);
	//返回指向首个大于给定键的元素的迭代器 -- <给定键>不一定要存在于map 中
	//所有key大于 n 的元素, 等于不算大于!!
	it = x1.upper_bound(70);
	if(it != x1.end())
		printf("13 -- upper_bound(70) = %d-%d\n", it->first,it->second);



	//14.一个删除区域元素的demo:
	/*
		下限包含3=30, 即小于, 等于. 上限排除等于, 走向更高的7-70.
		但实际删除区域即3-7. 连同6-60 一起删除.
		如果连用两个lower_bound, 不能删除6-60.
		连用两个upper_bound, 则3-30不能删除.
		反过来用, 则3-30, 6-60 都不会删除.
	 */
	for(tmp=0; tmp<10; tmp++)
		mymap[tmp]=tmp*10;							// 10 20 30 40 50 60 70 80 90
	itlow=mymap.lower_bound(3);				//			 ^
	itup=mymap.upper_bound(6);				//									 ^
	mymap.erase(itlow,itup);					// 10 20 70 80 90

	//打印区域删除后的结果
	cout << "14 -- mymap: ";
	if(!mymap.empty()){
		for (it=mymap.begin(); it!=mymap.end(); it++)
			cout << (*it).first << "-"<< (*it).second << "  ";
		cout << endl;
	}



	//*********
	//比较类方法
	//*********
	//15.key_compare key_comp();//key_comp键比较(返回用于比较键的函数)
	/*
		[实际就是拷贝出所有的key来, 再逐一遍历比较;
		在map中, key!=value, key_compare!=value_compare]
		不过, 你取出key_compare 之后, 基本上也可以打印出value_compare!!
		真的不知道再取value_compare 有什么意义.
	 */
	kcomp = x1.key_comp();//取出所有的key(其实也包含所有的value)

	//打印-所有的key(其实也包含打印所有的value)
	tmp=x1.rbegin()->first;
	it=x1.begin();
	printf("15 -- ");
	do{
		cout << (*it).first << "-"<< (*it).second << "  ";
	}while(kcomp((*it++).first,tmp));
	cout << endl;

	//暂时不知道有什么用, 而且这个用法是错误的.
	//上一个方法, 已经可以取出key and value, 真的不知道这个方法有何用？？
	/*
	//value_comp 值比较--返回用于在value_type类型的对象中比较键的函数
	//[实际就是拷贝出value 来, 但在map 中, key = value]
	map<int,int>::key_compare vomp;
	vomp = x1.value_comp();

	//打印
	int phighest=*x1.rbegin()->second;
	it=x1.begin();
	do{
		cout << "key=" << (*it).first << " val="<< (*it).second << endl;
	}while(kcomp((*it++).second,phighest));
	printf("\n\n");
	*/


	return 0;
}



