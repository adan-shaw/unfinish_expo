/*
 * _array_queue.cpp
 *
 *  Created on: Aug 16, 2017
 *      Author: adan
 */

#include <string.h>
#include <assert.h>
#include "_array_queue.h"
#include <math.h>
//构造函数
_array_queue::_array_queue(void){
  this->aq_reset();//初始化全局变量结构体aq_info
  //this->aq_test();//执行模块自测
}
//
//释构函数
_array_queue::~_array_queue(void){
  //销毁锁
  assert(pthread_rwlock_destroy(&this->aq.aq_rwlock) == 0);
}
//
//重置函数(即使pop 空, 有机会还原容器状态, 也不要还原, for test)
void _array_queue::aq_reset(void){
  struct aq_info* paq = &this->aq;
  paq->aq_cur_size = 0;
  paq->aq_stat = aq_ok;
  paq->aq_test_err_count = 0;
  paq->aq_cur_pos_front = 0;
  paq->aq_cur_pos_rear = 0;
  pthread_rwlock_init(&paq->aq_rwlock,NULL);

  memset(paq->aq_buf,aq_err_val_init,sizeof(aq_element_type)*aq_max_buf);
  //memset(paq->aq_buf_pushN,0,sizeof(aq_element_type)*aq_max_buf_pushN);
  return;
}

//压队列一个元素:
//成功aq_stat = aq_ok, 返回true
//失败aq_stat = aq_err_queue_full, 返回false
bool _array_queue::aq_push(aq_element_type val, bool push_rear){
  struct aq_info* paq = &this->aq;
  //插入安检
  if( (val <= aq_max_val_low) && (val >= aq_max_val_high) ){
    pthread_rwlock_wrlock(&paq->aq_rwlock);
    paq->aq_stat = aq_err_val_overflow;
    paq->aq_test_err_count++;
    pthread_rwlock_unlock(&paq->aq_rwlock);
    return false;
  }

  if(!this->aq_check_full(1))
    return false;//队列满检查失败

  movePos_doPush(&val, 1, push_rear);//移动游标, 执行push
  return true;
}
//弹出一个元素:
//成功aq_stat = aq_ok, 返回true
//失败aq_stat = aq_err_queue_empty, 返回false
bool _array_queue::aq_pop(aq_element_type *ret_val, bool pop_front){
  int ret_cur_size = 0;
  if(!this->aq_check_empty(&ret_cur_size))
    return false;//队列空检查失败

  movePos_doPop(ret_val, 1, pop_front);//移动游标, 执行pop
  return true;
}
//
//返回队列的当前的元素个数
void _array_queue::aq_get_cur_size(int *cur_size){
  struct aq_info* paq = &this->aq;
  pthread_rwlock_rdlock(&this->aq.aq_rwlock);
  *cur_size = paq->aq_cur_size;
  paq->aq_stat = aq_ok;
  pthread_rwlock_unlock(&this->aq.aq_rwlock);
}
//压队列N个元素(不支持最大限度插入):
//成功aq_stat = aq_ok, 返回true
//失败aq_stat = aq_err_queue_full, 返回false
bool _array_queue::aq_pushN(aq_element_type *val_arr, int N, bool push_rear){
  assert(val_arr != NULL);//必须保证指针不为空, 否则程序就应该终止了
  struct aq_info* paq = &this->aq;
  //插入安检
  int i = 0;
  for(;i < N;i++){
    if( (val_arr[i] <= aq_max_val_low) && (val_arr[i] >= aq_max_val_high) ){
      pthread_rwlock_wrlock(&this->aq.aq_rwlock);
      paq->aq_stat = aq_err_val_overflow;
      paq->aq_test_err_count++;
      pthread_rwlock_unlock(&this->aq.aq_rwlock);
      return false;//只要有一个值是错误的, 就不执行插入
    }
  }

  if(!this->aq_check_full(N))
    return false;//队列满检查失败

  //通过批量插入条件, 执行正常插入
  movePos_doPush(val_arr, N, push_rear);//移动游标, 执行push
  return true;
}
//弹出N个元素[N<=0, 提空队列]:
//成功aq_stat = aq_ok, 返回true
//失败aq_stat = aq_err_queue_empty, 返回false
bool _array_queue::aq_popN(aq_element_type *re_val_arr, int N, bool pop_front){
  assert(re_val_arr != NULL);//必须保证指针不为空, 否则程序就应该终止了

  int ret_cur_size = 0;
  if(!this->aq_check_empty(&ret_cur_size))
    return false;//队列空检查失败

  if(N <= 0)//N <= 0, 提空队列
    N = ret_cur_size;

  if(N > ret_cur_size){//队列没有那么多元素
    //paq->aq_stat = aq_err_queue_poor;
    N = ret_cur_size;
  }

  movePos_doPop(re_val_arr, N, pop_front);//移动游标, 执行pop
  return true;
}
//
//检查队列有没有重大错误
void _array_queue::aq_check(int *cur_err_count){
  struct aq_info* paq = &this->aq;
  pthread_rwlock_rdlock(&this->aq.aq_rwlock);
  *cur_err_count = paq->aq_test_err_count;
  pthread_rwlock_unlock(&this->aq.aq_rwlock);
}
//
//if 空队列
bool _array_queue::aq_check_empty(int *ret_cur_size){
  //判断是否队列空
  struct aq_info* paq = &this->aq;

  pthread_rwlock_rdlock(&this->aq.aq_rwlock);
  int aq_cur_size = paq->aq_cur_size;
  //int x = abs((paq->aq_cur_pos_front - paq->aq_cur_pos_rear));
  pthread_rwlock_unlock(&this->aq.aq_rwlock);
  *ret_cur_size = aq_cur_size;

  /*
  if(aq_cur_size != x){//队列异常--纠正, 退出
    pthread_rwlock_wrlock(&this->aq.aq_rwlock);
    paq->aq_stat = aq_err_frontRear_curSize;
    paq->aq_test_err_count++;
    paq->aq_cur_size = x;//丢值, 修改
    pthread_rwlock_unlock(&this->aq.aq_rwlock);
    return false;
  }
  */

  if(aq_cur_size <= aq_sig_pos_empty){
    pthread_rwlock_wrlock(&this->aq.aq_rwlock);
    paq->aq_stat = aq_err_queue_empty;//队列空
    if(aq_cur_size != aq_sig_pos_empty){
      paq->aq_stat = aq_err_queue_emptyEx;//队列空且溢出(错误)
      paq->aq_test_err_count++;
      paq->aq_cur_size = aq_sig_pos_empty;//丢值, 修改
    }
    pthread_rwlock_unlock(&this->aq.aq_rwlock);
    return false;
  }

  if(aq_cur_size > aq_max_buf){
    pthread_rwlock_wrlock(&this->aq.aq_rwlock);
    paq->aq_stat = aq_err_exception;
    paq->aq_test_err_count++;
    paq->aq_cur_size = aq_max_buf;//丢值, 修改
    pthread_rwlock_unlock(&this->aq.aq_rwlock);
    return false;
  }

  return true;
}
//
//if 满队列
bool _array_queue::aq_check_full(int N){
  //判断是否队列满
  struct aq_info* paq = &this->aq;
  pthread_rwlock_rdlock(&this->aq.aq_rwlock);
  int aq_cur_size = paq->aq_cur_size;
  //int x = abs((paq->aq_cur_pos_front - paq->aq_cur_pos_rear));
  pthread_rwlock_unlock(&this->aq.aq_rwlock);

  /*
  if(aq_cur_size != x){//队列异常--纠正, 退出
    pthread_rwlock_wrlock(&this->aq.aq_rwlock);
    paq->aq_stat = aq_err_frontRear_curSize;
    paq->aq_test_err_count++;
    paq->aq_cur_size = x;//丢值, 修改
    pthread_rwlock_unlock(&this->aq.aq_rwlock);
    return false;
  }
  */
  //<aq_max_buf 互改 aq_sig_pos_full>
  if(aq_cur_size >= aq_max_buf){
    pthread_rwlock_wrlock(&this->aq.aq_rwlock);
    paq->aq_stat = aq_err_queue_full;//队列满
    if(aq_cur_size != aq_max_buf){
      paq->aq_stat = aq_err_queue_fullEx;//队列满且溢出(错误)
      paq->aq_test_err_count++;
      paq->aq_cur_size = aq_max_buf;//丢值, 修改
    }
    pthread_rwlock_unlock(&this->aq.aq_rwlock);
    return false;
  }

  if(aq_cur_size < aq_sig_pos_empty){
    pthread_rwlock_wrlock(&this->aq.aq_rwlock);
    paq->aq_stat = aq_err_exception;
    paq->aq_test_err_count++;
    paq->aq_cur_size = aq_sig_pos_empty;//丢值, 修改
    pthread_rwlock_unlock(&this->aq.aq_rwlock);
    return false;
  }

  int x = aq_cur_size + N;
  if(x > aq_max_buf){//防止该次批量插入的量大于队列当前的容纳量
    pthread_rwlock_wrlock(&this->aq.aq_rwlock);
    paq->aq_stat = aq_err_queue_full;
    paq->aq_test_err_count++;
    pthread_rwlock_unlock(&this->aq.aq_rwlock);
    return false;
  }
  return true;
}

//既然有容量肯定不会出错的, 直接纠正游标问题
//移动游标, 执行push
void _array_queue::movePos_doPush(aq_element_type *val_arr, int N, bool push_rear){
  struct aq_info* paq = &this->aq;
  pthread_rwlock_rdlock(&paq->aq_rwlock);
  int aq_cur_pos_front = paq->aq_cur_pos_front;
  int aq_cur_pos_rear = paq->aq_cur_pos_rear;
  //int aq_cur_size = paq->aq_cur_size;
  pthread_rwlock_unlock(&paq->aq_rwlock);

  int sum_N = 0;
  int overflow = 0;//溢出差
  int i = 0;
  pthread_rwlock_wrlock(&paq->aq_rwlock);
  if(push_rear){//队尾插入
    sum_N = aq_cur_pos_rear + N;//PUSH 入之后的游标是否溢出
    if(sum_N > aq_sig_pos_full){//需要调头
      overflow = sum_N - aq_sig_pos_full - 1;//求得溢出容量差

      for(i = 0; aq_cur_pos_rear <= aq_sig_pos_full; i++, aq_cur_pos_rear++, paq->aq_cur_pos_rear++)
        paq->aq_buf[aq_cur_pos_rear] = val_arr[i];//未溢出部分继续加

      int x = i;//val_arr 数组位置
      paq->aq_cur_pos_rear = 0;//执行调头
      aq_cur_pos_rear = 0;

      for(i = 0;i < overflow;i++,x++,aq_cur_pos_rear++,paq->aq_cur_pos_rear++)
        paq->aq_buf[aq_cur_pos_rear] = val_arr[x];//溢出移到队首
    }
    else{//不用调头
      for(i = 0;aq_cur_pos_rear < sum_N;aq_cur_pos_rear++, i++, paq->aq_cur_pos_rear++)
        paq->aq_buf[aq_cur_pos_rear] = val_arr[i];//直接插入
    }
  }
  else{//队头插入
    sum_N = aq_cur_pos_front - N;//PUSH 入之后的游标是否溢出
    if(sum_N < aq_sig_pos_empty){//需要调头
      overflow = abs(sum_N) - 1;//求得溢出容量差

      for(i = 0; aq_cur_pos_front >= aq_sig_pos_empty; i++, aq_cur_pos_front--, paq->aq_cur_pos_front--)
        paq->aq_buf[aq_cur_pos_front] = val_arr[i];//未溢出部分继续加

      int x = i;//val_arr 数组位置
      paq->aq_cur_pos_front = aq_sig_pos_full;//执行调头
      aq_cur_pos_front = aq_sig_pos_full;

      for(i = 0;i < overflow;i++,x++,aq_cur_pos_front--,paq->aq_cur_pos_front--)
        paq->aq_buf[aq_cur_pos_front] = val_arr[x];//溢出移到队首
    }
    else{//不用调头
      for(i = 0;aq_cur_pos_front > sum_N;aq_cur_pos_front--, i++, paq->aq_cur_pos_front--)
        paq->aq_buf[aq_cur_pos_front] = val_arr[i];//直接插入
    }
  }
  paq->aq_cur_size = paq->aq_cur_size + N;;//总数+N
  paq->aq_stat = aq_ok;
  pthread_rwlock_unlock(&paq->aq_rwlock);
  return;
}

//移动游标, 执行pop
void _array_queue::movePos_doPop(aq_element_type *re_val_arr, int N, bool pop_front){
  struct aq_info* paq = &this->aq;
  pthread_rwlock_rdlock(&paq->aq_rwlock);
  int aq_cur_pos_front = paq->aq_cur_pos_front;
  int aq_cur_pos_rear = paq->aq_cur_pos_rear;
  //int aq_cur_size = paq->aq_cur_size;
  pthread_rwlock_unlock(&paq->aq_rwlock);

  int sum_N = 0;
  int overflow = 0;//溢出差
  int i = 0;
  pthread_rwlock_wrlock(&paq->aq_rwlock);
  if(pop_front){//队头弹出
    sum_N = aq_cur_pos_front + N;
    if(sum_N > aq_sig_pos_full){//需要调头
      overflow = sum_N - aq_sig_pos_full - 1;//求得溢出容量差

      for(i = 0; aq_cur_pos_front <= aq_sig_pos_full; i++, aq_cur_pos_front++, paq->aq_cur_pos_front++){
        re_val_arr[i] = paq->aq_buf[aq_cur_pos_front];//未溢出部分继续加
        paq->aq_buf[aq_cur_pos_front] = aq_err_val_init;
      }

      int x = i;//val_arr 数组位置
      paq->aq_cur_pos_front = 0;//执行调头
      aq_cur_pos_front = 0;

      for(i = 0;i < overflow;i++,x++,aq_cur_pos_front++,paq->aq_cur_pos_front++){
        re_val_arr[i] = paq->aq_buf[aq_cur_pos_front];//溢出移到队首
        paq->aq_buf[aq_cur_pos_front] = aq_err_val_init;
      }
    }
    else{//不用调头
      for(i = 0;aq_cur_pos_front < sum_N;aq_cur_pos_front++, i++, paq->aq_cur_pos_front++){
        re_val_arr[i] = paq->aq_buf[aq_cur_pos_front];//直接插入
        paq->aq_buf[aq_cur_pos_front] = aq_err_val_init;
      }
    }
  }
  else{//队尾弹出
    sum_N = aq_cur_pos_rear - N;
    if(sum_N < aq_sig_pos_empty){//需要调头
      overflow = abs(sum_N) - 1;//求得溢出容量差

      for(i = 0; aq_cur_pos_rear >= aq_sig_pos_empty; i++, aq_cur_pos_rear--, paq->aq_cur_pos_rear--){
        re_val_arr[i] = paq->aq_buf[aq_cur_pos_rear];//未溢出部分继续加
        paq->aq_buf[aq_cur_pos_rear] = aq_err_val_init;
      }

      int x = i;//val_arr 数组位置
      paq->aq_cur_pos_rear = aq_sig_pos_full;//执行调头
      aq_cur_pos_rear = aq_sig_pos_full;

      for(i = 0;i < overflow;i++,x++,aq_cur_pos_rear--,paq->aq_cur_pos_rear--){
        re_val_arr[x] = paq->aq_buf[aq_cur_pos_rear];//溢出移到队首
        paq->aq_buf[aq_cur_pos_rear] = aq_err_val_init;
      }
    }
    else{//不用调头
      for(i = 0;aq_cur_pos_rear > sum_N;aq_cur_pos_rear--, i++, paq->aq_cur_pos_rear--){
        re_val_arr[i] = paq->aq_buf[aq_cur_pos_rear];//直接插入
        paq->aq_buf[aq_cur_pos_rear] = aq_err_val_init;
      }
    }
  }
  paq->aq_cur_size = paq->aq_cur_size - N;;//总数+N
  paq->aq_stat = aq_ok;
  pthread_rwlock_unlock(&paq->aq_rwlock);
  return;
}

//
//模块测试函数
void _array_queue::aq_test(void){
  printf("************************************\n");
  printf("array queue test start\n");
  printf("************************************\n");

  int test_err = 0;
  this->aq_reset();//初始化队列

  //获取队列当前容量=aq_buf_empty 测试
  int cur_size = -100;
  this->aq_get_cur_size(&cur_size);
  if(cur_size != 0){
    printf("array queue get cur_size=aq_buf_empty test fail\n");
    test_err++;
  }

  //空队列pop 测试
  aq_element_type aq_pop_x = 999;
  bool bb = this->aq_pop(&aq_pop_x,0);
  if(!(this->aq.aq_stat == aq_err_queue_empty && this->aq.aq_cur_size == aq_sig_pos_empty)){
    printf("array queue pop empty-queue test fail\n");
    test_err++;
  }

  //压入一个值测试
  aq_pop_x = 999;
  if(!(this->aq_push(aq_pop_x,0) && this->aq.aq_stat == aq_ok && this->aq.aq_buf[0] == aq_pop_x)){
    printf("array queue push one test fail\n");
    test_err++;
  }
  //pop 空测试
  aq_pop_x = 0;
  if(!(this->aq_pop(&aq_pop_x,0) && this->aq.aq_stat == aq_ok)){
    printf("array queue pop empty-queue test fail\n");
    test_err++;
  }

  //压入一个值测试
  aq_pop_x = 999;
  if(!(this->aq_push(aq_pop_x,1) && this->aq.aq_stat == aq_ok && this->aq.aq_buf[aq_sig_pos_full] == aq_pop_x)){
    printf("array queue push one test fail\n");
    test_err++;
  }
  //pop 空测试
  aq_pop_x = 0;
  if(!(this->aq_pop(&aq_pop_x,1) && this->aq.aq_stat == aq_ok)){
    printf("array queue pop empty-queue test fail\n");
    test_err++;
  }

  //将队列填满 测试--填不满就有容量缺陷
  int full_test;
  for(full_test = 0;full_test < aq_max_buf;full_test++){
    bb = this->aq_push(full_test,1);
    if(!(bb && this->aq.aq_stat == aq_ok && this->aq.aq_buf[full_test] == full_test)){
      printf("array queue full-all test fail\n");
      test_err++;
      printf("********%d\n",full_test);
      break;
    }
  }

  //获取队列当前容量=aq_max_buf 测试
  cur_size = -100;
  this->aq_get_cur_size(&cur_size);
  if(cur_size != aq_max_buf){
    printf("array queue get cur_size=aq_max_buf test fail\n");
    test_err++;
  }
  printf("xxxxx_now queue size = %d\n", cur_size);

  //满队列push 测试
  bb = this->aq_push(1000,1);
  if(!((!bb) && this->aq.aq_stat == aq_err_queue_full)){
    printf("array queue push full and one test fail\n");
    test_err++;
  }

  //将填满后的队列掏空
  int pop_test_count = aq_max_buf - 1;
  for(full_test = pop_test_count;full_test >= 0;full_test--){
    aq_pop_x = 0;
    if(!(this->aq_pop(&aq_pop_x,1) && this->aq.aq_stat == aq_ok)){
      printf("array queue pop a full-queue test fail\n");
      test_err++;
      printf("********%d\n",full_test);
    }
  }

  //pop 空测试
  aq_pop_x = 0;
  bb = this->aq_pop(&aq_pop_x,1);
  if(!((!bb) && this->aq.aq_stat == aq_err_queue_empty)){
    printf("array queue pop a full-queue and one test fail\n");
    test_err++;
  }

  //获取队列当前容量=aq_max_buf 测试
  cur_size = -100;
  this->aq_get_cur_size(&cur_size);
  if(cur_size != 0){
    printf("*****array queue get cur_size=0 test fail\n");
    test_err++;
  }
  printf("xxxxx_now queue size = %d\n", cur_size);

  //队列系统正常...这时候测试批量push 入是否正常...也方便提供使用案例
  int xx = 0;
  aq_element_type aq_arr[999];
  for(;xx < 999;xx++)
    aq_arr[xx] = xx;
  if(!this->aq_pushN(aq_arr,999,1) && this->aq.aq_stat == aq_ok){
    printf("aq_max_buf_pushN full-all tmp buf fail !!, now we full in %d times",xx);
    test_err++;
  }

  if(!(this->aq.aq_buf[0] == 0 && this->aq.aq_buf[998] == 999 && this->aq.aq_buf[1] == 1)){
    printf("批量插入值正确\n");
  }

  memset(aq_arr,0,sizeof(aq_element_type)*999);

  if(!this->aq_popN(aq_arr,999,1) && this->aq.aq_stat == aq_ok){
    printf("aq_max_buf_pushN full-all tmp buf fail !!, now we full in %d times",xx);
    test_err++;
  }

  if(!(aq_arr[0] == 0 && aq_arr[998] == 999 && aq_arr[1] == 1)){
    printf("批量弹出值正确\n");
  }

  //测试汇总
  if(test_err == 0)
    printf("********************array queue test ok************************\n");
  else
	printf("********************array queue test fail, please check it out why ************************\n");
  printf("important error notes print now: queue = %d\n", this->aq.aq_test_err_count);

  return;
}


