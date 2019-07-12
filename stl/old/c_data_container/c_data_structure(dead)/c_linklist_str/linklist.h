/*
 * linklist.h
 *
 *  Created on: Nov 4, 2017
 *      Author: adan
 */

//链表的作用: 做单词出现次数统计, 一个链表即可, 根本不用树结构
//你用树也是白费心机, 单词索引毫无排序可言, 直接char 比较, 链表更实际

//链表改造: 添加多一个string len 来快速区分单词的长度, 虽然插入单词的时候, 多了一个strlen 操作,
//但是每次索引的时候, 都可以减少很多无畏的strcmp 操作... 同一长度下的string, 应该也不多...
//起码减少了整个linklist 所有string 都要对比一次的消耗​​​​

#include <stdio.h>
#include <string.h>

#define ll_element_str_max 128

//链表结构体
struct linklist_element{
  void* buf;//字符串
  int buf_len;//字符串长度
  int count;//字符串出现次数统计
  struct linklist_element* front;//前后
  struct linklist_element* rear;

  struct linklist_element* first;//第一个(方便排序)***first 可以指代整个链表, 类似数组首地址指代整个数组
  struct linklist_element* last;//最后一个

};

//检测字符串长度函数(虽然很想做成宏, 但是不太可能, 也没有必要这样抠门吧)
//加入max 限制: 字符串长度不等超过128bit
int llcheck_strlen(char buf[]);

//链表初始化
void linklist_init(struct linklist_element* xlist);

//链表元素交换位置--仅排序时需要
void llswap(struct linklist_element* a,struct linklist_element* b);

//遍历修改first(所有元素first 更新--注意: 此时链表已经有序, 只是改值)
void traversal_xfirst(struct linklist_element* xlist, struct linklist_element* first);

//遍历修改last(所有元素last 更新--注意: 此时链表已经有序, 只是改值)
void traversal_xlast(struct linklist_element* xlist, struct linklist_element* last);

//插入时定位--从小到大排序
void llget_pos(struct linklist_element* xlist, struct linklist_element* himself);

//********************************************************************************
//对外接口函数, 其他除了linklist_init()外, 都是对内的
//********************************************************************************
/*
//< failure >
//吐血重封装-insert2: 每次insert 都要刷新链表首地址, 所以直接二次封装
void linklist_insert2(struct linklist_element* xlist, char* buf);
//吐血重封装-del2
void linklist_del2(struct linklist_element* xlist, char* buf);
*/

//********************************************************************************
//链表插入(被插入的字符串必须为malloc 出来的指针, 否则就会产生空指针)
void linklist_insert(struct linklist_element* xlist, char* buf);

//链表删除-单个删除
struct linklist_element* linklist_del(struct linklist_element* xlist, char* buf);

//链表清空(为了追求清空性能, 删除后是整个链表删除, 首个element 也被删除)
void linklist_del_all(struct linklist_element* xlist);

//链表重置(重置, 回到init 状态... 删除其余节点)
void linklist_reset(struct linklist_element* xlist);

//链表排序--根据字符串长度排序
//(从小到大, 简单冒泡即可-由于操作复杂, 而这个操作又不常用--其实基本用不到, 链表是从0 个开始插入的)
void linklist_sort(struct linklist_element* xlist, char* buf);

//链表查找(找到了, 返回该节点, 找不到, 返回NULL)
struct linklist_element* linklist_find(struct linklist_element* xlist, char* buf);

//获取链表总长度(元素的个数)--空链表返回0
int get_all_element_count(struct linklist_element* xlist);

//获取某个字符串的count 计数(成功返回count 个数, 失败返回-1)
int get_element_count(struct linklist_element* xlist, char* buf);

//自测函数
void linklist_test(void);

//********************************************************************************

