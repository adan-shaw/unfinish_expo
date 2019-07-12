/*
 * demo_aq.cpp
 *
 *  Created on: Jun 19, 2018
 *      Author: adan
 */

#include "../_array_queue.h"
#include <string.h>

void aq_demo(void){
  //1.创建类实体(数组太大, 最好用堆内存):
  _array_queue* paq = new _array_queue();

  //2.获取队列当前容量=aq_buf_empty 测试
  int cur_size = -100;
  paq->aq_get_cur_size(&cur_size);

  //********************************************
  //3.单个值操作(锁已经内置, 不用担心, 自动带读写锁)
  int aq_pop_x = 999;
  //队列头-压入一个值
  if(!(paq->aq_push(aq_pop_x,0)))
    printf("aq_push in queue front fail !!\n");
  aq_pop_x = 0;
  //队列尾-pop 一个值
  if(!(paq->aq_pop(&aq_pop_x,0)))
    printf("aq_pop in queue rear fail !!\n");

  //队列尾-压入一个值
  aq_pop_x = 999;
  if(!(paq->aq_push(aq_pop_x,1)))
    printf("aq_push in queue rear fail !!\n");
  //队列头-pop 一个值
  aq_pop_x = 0;
  if(!(paq->aq_pop(&aq_pop_x,1)))
    printf("aq_pop in queue front fail !!\n");

  //********************************************
  //4.批量操作
  //********************************************
  aq_element_type aq_arr[999];
  int xx = 0;
  for(;xx < 999;xx++)
    aq_arr[xx] = xx;
  //批量插入队列尾
  if(!paq->aq_pushN(aq_arr,999,1))
    printf("aq_pushN in queue rear fail !!\n");

  memset(aq_arr,0,sizeof(aq_element_type)*999);
  //批量从队列头弹出
  if(!paq->aq_popN(aq_arr,999,1))
    printf("aq_popN in queue front fail !!\n");

  //
  //5.释放类实体内存
  delete paq;
  return;
}
