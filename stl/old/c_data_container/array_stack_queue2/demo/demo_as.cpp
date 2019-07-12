/*
 * demo_as.cpp
 *
 *  Created on: Jun 19, 2018
 *      Author: adan
 */

#include "../_array_stack.h"
#include <string.h>

//模块测试函数
void as_demo(void){
  //1.创建类实体(数组太大, 最好用堆内存):
  _array_stack* paq = new _array_stack();

  //2.获取栈当前容量=as_buf_empty 测试
  int cur_size = -100;
  paq->as_get_cur_size(&cur_size);

  //********************************************
  //3.单个值操作(锁已经内置, 不用担心, 自动带读写锁)
  int as_pop_x = 999;
  //栈尾-压入一个值
  as_pop_x = 999;
  if(!(paq->as_push(as_pop_x)))
    printf("as_push in queue rear fail !!\n");
  //栈头-pop 一个值
  as_pop_x = 0;
  if(!(paq->as_pop(&as_pop_x))){
    printf("as_pop in queue front fail !!\n");
  }

  //********************************************
  //4.批量操作
  //********************************************
  as_element_type as_arr[999];
  int xx = 0;
  for(;xx < 999;xx++)
    as_arr[xx] = xx;
  //批量插入栈尾
  if(!paq->as_pushN(as_arr,999))
    printf("as_pushN in queue rear fail !!\n");

  memset(as_arr,0,sizeof(as_element_type)*999);
  //批量从栈头弹出
  if(!paq->as_popN(as_arr,999))
    printf("as_popN in queue front fail !!\n");

  //
  //5.释放类实体内存
  delete paq;
  return;
}
