//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -ggdb3 -o x ./bitset.cpp

//简介
//C++ bitset 一种<位操作-集合>的数据结构
//bitsets使用许多二元操作符, 比如逻辑和, 或等.

//bitset api list(略)



#include <stdio.h>
#include <string>
#include <bitset>
#include <iostream>
using namespace std;



//测试函数
//(函数不接受 '-' 字符串 , best 也是关键字)
int main(void){
	//1. 初始化<位集合>
	//将int = 32 bit 推如位容器, 即这个bitset 存在32 个元素.
	bitset<32> x1;
	bitset<32> x2(120ul);//unsigned long = 120

	bitset<32> x3(string("01011"));//initialize from string
	bitset<32> x4;
	bitset<32> xtmp;
	printf("1 -- \n");



	//2.bool any();//判断<位集合>是否有被置1 的位.
	//							<位集合>如果有任何一个位被置1, 就返回true
	if(x1.any())
		printf("2 -- x1<位集合>有一个位以上, 被置1\n");
	if(x3.any())
		printf("2 -- x3<位集合>有一个位以上, 被置1\n");



	//3.bool none();//判断<位集合>是否, 没有任何被置1 的位
	//								<位集合>没有任何一个位被置1, 就返回true
	if(x1.none())
		printf("3 -- x1<位集合>没有任何一个位被置1\n");
	if(x3.none())
		printf("3 -- x3<位集合>没有任何一个位被置1\n");



	//4.size_t count();//统计<位集合>被置1 的位数的count统计
	printf("4 -- x1<位集合>被置1 的位数count=%d\n",x1.count());
	printf("4 -- x3<位集合>被置1 的位数count=%d\n",x3.count());



	//5.size_t size();//求出<位集合>所有的位数, 必然等于初始化时候的32
	printf("5 -- x3<位集合>的位数初始化总和count=%d\n",x3.size());
	printf("5 -- x3<位集合>被置1 的位数count=%d,未被被置1 的位数count=%d\n",\
		x3.count(), x3.size()-x3.count());



	//6.bitset<N>& flip();//反转bits中的位, 返回一个新的bitset<N>
	cout << "6 -- x3=" << x3.to_string() << endl;
	x4 = x3.flip();
	x1 = x1.flip(31);//32 位的<位集合>, 求反最高是31 位 = 1.
	//x1 = x1.flip(string("10101010"));	//错误的定义, 字符串初始化,
																			//只能在<位集合>类构造的时候用
	cout << "6 -- x4 = x3.flip();x3=" << x3.to_string() << endl;
	cout << "6 -- x4 = x3.flip();x4=" << x4.to_string() << endl;
	cout << "6 -- x1 = x1.flip(31);x1=" << x1.to_string() << endl;
	//x1.flip(31);//错误的写法!!
	//cout << "6 -- 错误的写法!!x1.flip(31);x1=" << x1.to_string() << endl;
	//x3.flip();//无论是常值, 还是变量, 都不能用这种‘简式’写法.
	//cout << "6 -- 错误!!常值反位时, 可以用简式: x3.flip();x3="<<\
			x1.to_string() << endl;
	//一般情况下, 容器函数有返回, 就需要重新赋值, 接住修改结果.



	//7.to_string()转换<位集合>为二进制数据.(也可以看作是字符串数据)
	cout << "7 -- " << x4.to_string() << endl;
	cout << "7 -- " << x1.to_string() << endl;
	/*
		这里用cout是因为c语言printf()不能输出二进制,
		printf()只能输出8,10,16进制的数字和字符串
	*/
	//证明: <位集合>.to_string()不是输出字符串,而是二进制数据.
	printf("7 -- x4.to_string()=%s\n",x4.to_string());
	printf("7 -- x1.to_string()=%s\n",x1.to_string());



	//8.to_ulong() 转换<位集合>为unsigned long
	printf("8 -- x4.to_ulong()=%d\n",x4.to_ulong());
	printf("8 -- x1.to_ulong()=%d\n",x1.to_ulong());



	//9.bitset<N>& reset();	重置某个位为0
	xtmp = x4.reset();//默认全部清0
	cout <<"9 -- x4.reset()" << xtmp.to_string() << endl;
	xtmp = x1.reset(31);//只清0 第31 位
	cout <<"9 -- x1.reset()" << xtmp.to_string() << endl;



	//10.bitset<N>& set();//重置某个位为1
	xtmp = x1.set(2);//重置第二位为1
	cout <<"10 -- xtmp = x1.set(2):" << xtmp.to_string() << endl;
	xtmp = x1.set(0);//将0 位也置1
	cout <<"10 -- xtmp = x1.set(0):" << xtmp.to_string() << endl;



	//11.bool test(size_t pos);	如果目标位为1, 则返回true
	if(x1.test(2))
		printf("11 -- x1.test(2) = 1 = true\n");
	if(x1.test(1))
		printf("11 -- x1.test(1) = 1 = true\n");
	if(x1.test(0))
		printf("11 -- x1.test(0) = 1 = true\n");


	return 0;
}




