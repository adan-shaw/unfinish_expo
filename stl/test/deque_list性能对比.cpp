//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,

//编译:
//g++ -ggdb3 -o x ./deque_list性能对比.cpp


//1.list, deque 性能对比: deque 性能要强很多.
/*
	事实证明, 大规模的pop操作会使list慢得像吃屎, 这让我很不解.
	(大规模: 千万级别每秒的操作次数, 才会变慢.list 本身还是擅长删除和增加的)

	一开始我是不明白deque的删除操作为何效率如此之高, 我现在大概明白了.
	当deque执行pop_back()操作时, 只是将末尾的迭代器向前移动一位;
	deque 的删除和添加操作的时间花费是一样的.


	而对于list来说, 删除和添加节点的时间花费是一样的.
	因为链表怎样操作, 性能消耗都是平均的.


	对于简单的增删操作来说,deque的性能要优于list,而且可以减少碎片内存.
	但是list提供更高级的功能,deque则不具备高级功能;
	list()则提供sort()排序,unique()取出重复等高级功能.

	对于查找值:
	deque查找只能顺序遍历,deque容器也不提供查找find()API.

	list 可以使用std::find()函数做查找.
*/



//测试结论1:
/*
	迭代器如果只删除/插入一个元素, 性能差, 删除/插入多个元素, 性能好.
	迭代器适合批量删除/插入, 单个删除/插入,
	还是直接pop or push 性能最好.
*/

//测试结论2:
/*
deque pop_front or pop_back根本不影响性能!!
deque push_front or push_back根本不影响性能!!
front and back 都有游标记录的, 你会做, 别人也会做.
这一点不奇怪.
*/



//2.测试代码:
#include <deque>
#include <list>
#include <stdio.h>
#include <time.h>
using namespace std;



#define TEST_COUNT 9999999



//1.测试list增加节点性能
void list_add_node(list<int>* plist);

//2.测试list删除节点性能
void list_del_node(list<int>* plist);

//3.测试deque增加节点性能
void deque_add_node(deque<int>* pdeq);

//4.测试deque删除节点性能
void deque_del_node(deque<int>* pdeq);



int main(void){
	list<int> mlist;
	deque<int> mdeq;
	long int s,e;



	if(mlist.max_size() < TEST_COUNT*3){
		printf("max_size()=%d < TEST_COUNT*3=%d\n",\
				mlist.max_size(),TEST_COUNT*3);
		return -1;
	}
	//1.测试list增加节点性能
	printf("1 -- list_add_node() start:%d\n",s=time(NULL));
	list_add_node(&mlist);
	printf("1 -- list_add_node() finish:%d\n",e=time(NULL));
	printf("1 -- difftime()=%lf\n\n",difftime(e,s));

	//2.测试list删除节点性能
	printf("2 -- list_del_node() start:%d\n",s=time(NULL));
	list_del_node(&mlist);
	printf("2 -- list_del_node() finish:%d\n",e=time(NULL));
	printf("2 -- difftime()=%lf\n\n",difftime(e,s));

	//3.测试deque增加节点性能
	printf("3 -- deque_add_node() start:%d\n",s=time(NULL));
	deque_add_node(&mdeq);
	printf("3 -- deque_add_node() finish:%d\n",e=time(NULL));
	printf("3 -- difftime()=%lf\n\n",difftime(e,s));

	//4.测试deque删除节点性能
	printf("4 -- deque_del_node() start:%d\n",s=time(NULL));
	deque_del_node(&mdeq);
	printf("4 -- deque_del_node() finish:%d\n",e=time(NULL));
	printf("4 -- difftime()=%lf\n\n",difftime(e,s));

	return 0;
}



//1.测试list增加节点性能
void list_add_node(list<int>* plist){
	int tmp;
	list<int>::iterator it;



	for(tmp=0,it=plist->begin();tmp<TEST_COUNT;tmp++){
		plist->insert(it,tmp);
		plist->push_back(tmp);
		plist->push_front(tmp);
	}
	return;
}



//2.测试list删除节点性能
void list_del_node(list<int>* plist){
	int tmp;
	list<int>::iterator it;



	for(tmp=0,it=plist->begin();tmp<TEST_COUNT;tmp++){
		/*
		if(it != plist->end()){
			//plist->erase(it);
			;
		}
		else{
			it=plist->begin();
		}
		*/
		plist->pop_back();
		plist->pop_back();
		plist->pop_front();

	}
	return;
}



//3.测试deque增加节点性能
void deque_add_node(deque<int>* pdeq){
	int tmp;
	deque<int>::iterator it;



	for(tmp=0,it=pdeq->begin();tmp<TEST_COUNT;tmp++){
		//pdeq->push_front(tmp);
		//pdeq->push_front(tmp);
		//pdeq->push_front(tmp);
		pdeq->push_back(tmp);
		pdeq->push_back(tmp);
		pdeq->push_back(tmp);
	}
	return;
}



//4.测试deque删除节点性能
void deque_del_node(deque<int>* pdeq){
	int tmp;
	deque<int>::iterator it;



	for(tmp=0,it=pdeq->begin();tmp<TEST_COUNT;tmp++){
		pdeq->pop_back();
		pdeq->pop_back();
		pdeq->pop_front();
	}
	return;
}


