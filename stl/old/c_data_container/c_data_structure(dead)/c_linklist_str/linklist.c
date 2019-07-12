/*
 * linklist.c
 *
 *  Created on: Nov 4, 2017
 *      Author: adan
 */


#include "linklist.h"


//检测字符串长度函数(虽然很想做成宏, 但是不太可能, 也没有必要这样抠门吧)
//加入max 限制: 字符串长度不等超过128bit
int llcheck_strlen(char buf[]){
  int tmp = 0;
  while(buf[tmp] != '\0' && tmp < ll_element_str_max){
	//printf("%c, %d",buf[tmp],tmp);
	tmp++;
  }
  if(tmp > ll_element_str_max)
	printf("warning!! check_strlen result > %d, string is too long !!\n", ll_element_str_max);
  return ++tmp;
}

//链表初始化
void linklist_init(struct linklist_element* xlist){
  xlist->buf = NULL;
  xlist->buf_len = NULL;
  xlist->count = NULL;
  xlist->front = NULL;
  xlist->rear = NULL;
  xlist->first = NULL;
  xlist->last = NULL;
}

//链表元素交换位置--仅排序时需要
void llswap(struct linklist_element* a,struct linklist_element* b){
  //执行交换
  struct linklist_element* front_tmp = a->front;
  struct linklist_element* rear_tmp = a->rear;
  a->front = b->front;
  a->rear = b->rear;
  b->front = front_tmp->front;
  b->rear = rear_tmp->rear;
  return;
}

//遍历修改first(所有元素first 更新--注意: 此时链表已经有序, 只是改值)
void traversal_xfirst(struct linklist_element* xlist, struct linklist_element* first){
  struct linklist_element* tmp = xlist;
  while(tmp->last != tmp && tmp->rear != tmp && tmp->rear != NULL){//非最后一个
	tmp->first = first;
	tmp = tmp->rear;//指向下一个
  }
  //最后一个
  tmp->first = first;
  return ;
}

//遍历修改last(所有元素last 更新--注意: 此时链表已经有序, 只是改值)
void traversal_xlast(struct linklist_element* xlist, struct linklist_element* last){
  struct linklist_element* tmp = xlist;
  while(tmp->last != tmp && tmp->rear != tmp && tmp->rear != NULL){//非最后一个
	tmp->last = last;
	tmp = tmp->rear;//指向下一个
  }
  //最后一个
  tmp->last = last;
  return ;
}


//插入时定位
void llget_pos(struct linklist_element* xlist, struct linklist_element* himself){
  if(himself->buf_len <= xlist->buf_len){
	//最前面插入(第一个=链表指针=str_len mix)
	himself->rear = xlist;
	himself->front = himself;
	xlist->front = himself;

	traversal_xfirst(himself, himself);//所有人更新first
	himself->last = xlist->last;
	return;
  }
  if(himself->buf_len > xlist->last->buf_len){
    //最后面插入(最后一个=str_len max)
	himself->front = xlist->last;
	himself->rear = himself;
	xlist->last->rear = himself;

	himself->first = xlist->first;
	traversal_xlast(himself->first, himself);
    return;
  }

  //不能用递归
  struct linklist_element* tmp = xlist->rear;
  while(tmp->rear != xlist->last && tmp->rear != tmp && tmp->rear != NULL){
	if(himself->buf_len <= tmp->buf_len){
	  tmp->front->rear = himself;
	  himself->front = tmp->front->rear;
	  himself->rear = tmp;
	  tmp->front = himself;

	  himself->first = tmp->first;
	  himself->last = tmp->last;
	  return;
	}
	else
	  tmp = tmp->rear;//后移
  }

  printf("llget_pos impossible error!!\n");
  return;
}

/*
//< 失败 >
//吐血重封装-insert2: 每次insert 都要刷新链表首地址, 所以直接二次封装
void linklist_insert2(struct linklist_element* xlist, char* buf){
  linklist_insert(xlist,buf);
  xlist = xlist->front;
}
//吐血重封装-del2
void linklist_del2(struct linklist_element* xlist, char* buf){
  linklist_del2(xlist,buf);
  xlist = xlist->front;
}
*/

//链表插入(被插入的字符串必须为malloc 出来的指针, 否则就会产生空指针)
void linklist_insert(struct linklist_element* xlist, char* buf){
  //判断关键元素, 绝对不能有一个掉对 for test(这种机会还是没什么可能的, 最大可能是指针赋值错误, 出现空指针)
  int str_len = llcheck_strlen(buf);
  if(str_len == 0 || str_len >= ll_element_str_max){//不允许插入非法操作
	printf("error operation !!insert a bad string, str_len = %d, string = %s\n", str_len, buf);
	return;
  }

  //查看链表中是否已经存在该元素
  struct linklist_element* is_exist = NULL;
  if(xlist->front && xlist->rear && xlist->first && xlist->last){//链表不为空
	is_exist = linklist_find(xlist, buf);
  }
  if(is_exist != NULL){//存在, 计数+1 就退出
	is_exist->count++;
	return;
  }

  //元素不存在, 新插入元素
  if(!(xlist->front && xlist->rear && xlist->first && xlist->last)){//整个链表第一次插入string
	xlist->buf = buf;
	xlist->buf_len = str_len;
	xlist->count = 1;

	xlist->first = xlist;//固定的
	xlist->front = xlist;
	xlist->rear = xlist;
	xlist->last = xlist;
	return;
  }
  else{//链表元素count != 0(非第一次插入)
	//不存在, 新增, 继续向下走
	struct linklist_element* tmp;
	tmp = malloc(sizeof(struct linklist_element));//创建新节点
	tmp->buf = buf;
	tmp->buf_len = str_len;
	tmp->count = 1;

	llget_pos(xlist,tmp);//插入时定位(自己, 第一个, 第二个)
	return;
  }
  printf("linklist_insert impossible error!!\n");
  return;
}

//链表删除-单个删除
struct linklist_element* linklist_del(struct linklist_element* xlist, char* buf){
  //获取字符串长度
  int str_len = llcheck_strlen(buf);
  if(str_len == 0 || str_len >= ll_element_str_max){//不允许插入非法操作
	printf("linklist_del error operation !!str_len = %d, string = %s\n", str_len, buf);
	return NULL;
  }

  //查看链表中是否已经存在该元素
  struct linklist_element* is_exist = NULL;
  if(xlist->front && xlist->rear && xlist->first && xlist->last){//链表不为空
	is_exist = linklist_find(xlist, buf);
  }
  if(is_exist != NULL){//存在, 执行删除
	struct linklist_element* front = is_exist->front;
	struct linklist_element* rear = is_exist->rear;

	free(is_exist->buf);//先删除字符串
	free(is_exist);//释放自己的内存

	if(front == is_exist){//删除头
      //要返回新的链表头, 重新赋值
	  return rear;
	}
	else if(rear == is_exist){//删除尾
      //直接删除
	  front->rear = front;//封尾
	  return NULL;
	}
	else{
	  //执行断链
	  front->rear = rear;
	  rear->front = front;
	  return NULL;
	}
  }
  printf("error operation !!linklist has no this string, str_len = %d, string = %s\n", str_len, buf);
  return NULL;
}

//链表清空(为了追求清空性能, 删除后是整个链表删除, 首个element 也被删除)
void linklist_del_all(struct linklist_element* xlist){
  int del_count = get_all_element_count(xlist);
  struct linklist_element* tmp = xlist;
  struct linklist_element* save_tmp;
  while(del_count > 0){//非最后一个
	free(tmp->buf);//释放字符串
	save_tmp = tmp;
	tmp = tmp->rear;//指向下一个
	free(save_tmp);//释放自己的内存
	--del_count;
  }
  //free(tmp->buf);//释放最后一个字符串
  //free(tmp);
  //xlist = NULL;
  return;
}

//链表重置(回到init 状态)
void linklist_reset(struct linklist_element* xlist){
  int del_count = get_all_element_count(xlist);
  struct linklist_element* tmp = xlist;
  struct linklist_element* save_tmp;
  while(del_count > 0){//非最后一个
	free(tmp->buf);//释放字符串
	save_tmp = tmp;
	tmp = tmp->rear;//指向下一个
	free(save_tmp);//释放自己的内存
	--del_count;
  }
  //free(tmp->buf);//释放最后一个字符串
  //linklist_init(tmp);//重置自己
  return;
}

//链表排序--根据字符串长度排序
//(从小到大, 简单冒泡即可-由于操作复杂, 而这个操作又不常用--其实基本用不到, 链表是从0 个开始插入的)
void linklist_sort(struct linklist_element* xlist, char* buf){

}

//链表查找(找到了, 返回该节点, 找不到, 返回NULL)
struct linklist_element* linklist_find(struct linklist_element* xlist, char* buf){
  int str_len = llcheck_strlen(buf);//获取字符串长度
  struct linklist_element* tmp = xlist;
  while(tmp->last != tmp && tmp->rear != tmp && tmp->rear != NULL){//不是最后一个
    if(tmp->buf_len == str_len){
      //字符串长度相同, 判断字符串是否匹配
      if(strcmp(tmp->buf,buf) == 0)
        return tmp;
      else
    	tmp = tmp->rear;
    }
    else
      tmp = tmp->rear;
  }
  //检查最后一个
  if(tmp->buf_len == str_len){
    if(strcmp(tmp->buf,buf) == 0)//字符串长度相同, 判断字符串是否匹配
      return tmp;//最后一个中了
    else
      return NULL;//最后一个都不中
    }
  else
	return NULL;
  return NULL;//防止出错
}

//获取链表总长度(元素的个数)--空链表返回0
int get_all_element_count(struct linklist_element* xlist){
  int count = 0;
  struct linklist_element* tmp = xlist;
  while(tmp->last != tmp && tmp->rear != tmp && tmp->rear != NULL){//非最后一个
	++count;
	tmp = tmp->rear;//指向下一个
  }
  //加上最后一个
  if(tmp->rear != NULL)
    return ++count;
  else
	return count;
}

//获取某个字符串的count 计数(成功返回count 个数, 失败返回-1)
int get_element_count(struct linklist_element* xlist, char* buf){
  struct linklist_element* tmp = linklist_find(xlist,buf);
  if(tmp == NULL)
	return -1;
  else
	return tmp->count;
}

//自测函数
void linklist_test(void){
  char x1[] = "abvvv";
  char x2[] = "cca";
  char x3[] = "ab";
  char x4[] = "cc";
  char x5[] = "a";
  char x6[] = "b";
  char x7[] = "a9999999999999999999999";
  char* str_tmp1 = malloc(sizeof(x1));
  strcpy(str_tmp1,x1);
  char* str_tmp2 = malloc(sizeof(x2));
  strcpy(str_tmp2,x2);
  char* str_tmp3 = malloc(sizeof(x3));
  strcpy(str_tmp3,x3);
  char* str_tmp4 = malloc(sizeof(x4));
  strcpy(str_tmp4,x4);
  char* str_tmp5 = malloc(sizeof(x5));
  strcpy(str_tmp5,x5);
  char* str_tmp6 = malloc(sizeof(x6));
  strcpy(str_tmp6,x6);
  char* str_tmp7 = malloc(sizeof(x7));
  strcpy(str_tmp7,x7);

  printf("before:\n");
  printf("%s-%d\n",str_tmp1,1);
  printf("%s-%d\n",str_tmp2,1);
  printf("%s-%d\n",str_tmp3,1);
  printf("%s-%d\n",str_tmp4,1);
  printf("%s-%d\n",str_tmp5,1);
  printf("%s-%d\n",str_tmp6,1);
  printf("%s-%d\n",str_tmp7,1);

  //先创建一个节点, 无论你是否使用, 也要先创建一个节点作为母体, 然后执行初始化
  struct linklist_element* xlist = malloc(sizeof(struct linklist_element));
  void linklist_init(struct linklist_element* xlist);//链表初始化

  int ctmp = 0;
  //执行插入操作会自己区分是否是第一次插入


  for(;ctmp<10;ctmp++){//插入10次
    linklist_insert(xlist, str_tmp1);
    xlist = xlist->front;
  }
  //插入相同元素测试
  ctmp = get_all_element_count(xlist);
  if(ctmp != 1)
	printf("插入同样的元素10 次, 折叠失败, 现在元素总长度为:%d, shoule be 1...\n",ctmp);
  else
	printf("插入同样的元素10 次成功, 开始测试插入不同元素\n");

  //插入不同元素测试
  linklist_insert(xlist, str_tmp1);
  xlist = xlist->front;
  linklist_insert(xlist, str_tmp2);
  xlist = xlist->front;
  linklist_insert(xlist, str_tmp3);
  xlist = xlist->front;
  linklist_insert(xlist, str_tmp4);
  xlist = xlist->front;
  linklist_insert(xlist, str_tmp5);
  xlist = xlist->front;
  linklist_insert(xlist, str_tmp6);
  xlist = xlist->front;
  linklist_insert(xlist, str_tmp7);
  xlist = xlist->front;
  linklist_insert(xlist, str_tmp3);
  xlist = xlist->front;
  linklist_insert(xlist, str_tmp2);
  xlist = xlist->front;
  linklist_insert(xlist, str_tmp5);
  xlist = xlist->front;
  linklist_insert(xlist, str_tmp3);
  xlist = xlist->front;
  linklist_insert(xlist, str_tmp2);
  xlist = xlist->front;
  linklist_insert(xlist, str_tmp7);
  xlist = xlist->front;
  printf("插入不同元素测试 shoule be:\n");
  printf("%s-%d-%d\n",str_tmp1,get_element_count(xlist, str_tmp1),11);
  printf("%s-%d-%d\n",str_tmp2,get_element_count(xlist, str_tmp2),3);
  printf("%s-%d-%d\n",str_tmp3,get_element_count(xlist, str_tmp3),3);
  printf("%s-%d-%d\n",str_tmp4,get_element_count(xlist, str_tmp4),1);
  printf("%s-%d-%d\n",str_tmp5,get_element_count(xlist, str_tmp5),2);
  printf("%s-%d-%d\n",str_tmp6,get_element_count(xlist, str_tmp6),1);
  printf("%s-%d-%d\n",str_tmp7,get_element_count(xlist, str_tmp7),2);

  //重置测试
  linklist_reset(xlist);
  linklist_init(xlist);//重置自己
  ctmp = get_all_element_count(xlist);
  if(ctmp != 0)
	printf("重置测试失败, 现在元素总长度为:%d, shoule be 0...\n",ctmp);
  else
	printf("重置测试成功, 开始回归测试\n");

  str_tmp2 = malloc(sizeof(x2));
  strcpy(str_tmp2,x2);
  str_tmp7 = malloc(sizeof(x7));
  strcpy(str_tmp7,x7);

  for(ctmp=0;ctmp<10;ctmp++){//插入10次
    linklist_insert(xlist, str_tmp7);
    xlist = xlist->front;
  }

  //插入相同元素测试
  ctmp = get_all_element_count(xlist);
  if(ctmp != 1)
	printf("插入同样的元素10 次, 折叠失败, 现在元素总长度为:%d, shoule be 1...\n",ctmp);
  else
	printf("插入同样的元素10 次成功, 开始测试插入不同元素\n");

  linklist_insert(xlist, str_tmp2);
  xlist = xlist->front;
  printf("插入不同元素测试 shoule be:\n");
  printf("%s-%d\n",str_tmp2,get_element_count(xlist, str_tmp2));
  printf("%s-%d\n",str_tmp7,get_element_count(xlist, str_tmp7));
  printf("get_all_element_count = %d\n",get_all_element_count(xlist));

  //删除元素测试
  struct linklist_element* xdel = linklist_del(xlist,str_tmp2);
  if(xdel != NULL)//如果删了头就更新头
    xlist = xdel;
  ctmp = get_all_element_count(xlist);//获取链表总长度(元素的个数)--空链表返回0
  if(ctmp == 1)
	if(linklist_find(xlist,str_tmp2) == NULL)
	  printf("删除元素测试成功, 开始清空链表\n");
	else
	  printf("删除元素测试失败, linklist_find(xlist,%s) 成功???你都删了这个字符串了\n",str_tmp2);
  else
	  printf("删除元素测试失败, 现在元素总长度为:%d, shoule be 1...\n",ctmp);

  //清空链表
  linklist_del_all(xlist);

  if(xlist == NULL)
	printf("清空链表成功\n");
  else
	printf("清空链表成功\n");

  printf("all test finish !!\n");

  //释放资源(清空链表 会自动释放字符串)
  //free(str_tmp1);
  //free(str_tmp2);
  //free(str_tmp3);
  //free(str_tmp4);
  //free(str_tmp5);
  //free(str_tmp6);
  //free(str_tmp7);
  return ;
}
