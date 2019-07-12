/*
 * _array_stack.cpp
 *
 *  Created on: Aug 16, 2017
 *      Author: adan
 */

#include "_array_stack.h"
#include <string.h>
#include <assert.h>

//构造函数
_array_stack::_array_stack(void){
  this->as_reset();//初始化全局变量结构体as_info
  //this->as_test();//执行模块自测
}
//
//释构函数
_array_stack::~_array_stack(void){

}
//
//重置函数(即使pop 空, 有机会还原容器状态, 也不要还原, for test)
void _array_stack::as_reset(void){
  struct as_info* pas = &this->as;
  pas->as_cur_size = -1;//栈比较特殊,这里不能为0, 设计为-1 可以减少一个if 的使用
  pas->as_stat = as_ok;
  pas->as_test_err_count = 0;

  memset(pas->as_buf,aq_err_val_init,sizeof(as_element_type)*as_max_buf);
  //memset(pas->as_buf_pushN,0,sizeof(as_element_type)*as_max_buf_pushN);
  return;
}
//压栈一个元素:
//成功as_stat = as_ok, 返回true
//失败as_stat = as_err_stack_full, 返回false
bool _array_stack::as_push(as_element_type val){
  struct as_info* pas = &this->as;
  //插入安检
  if( (val <= as_max_val_low) && (val >= as_max_val_high) ){
    pas->as_stat = as_err_val_overflow;
    pas->as_test_err_count++;
    return false;
  }

  if(!this->as_check_full())
    return false;//栈满检查失败

  pas->as_cur_size++;
  pas->as_buf[pas->as_cur_size] = val;
  pas->as_stat = as_ok;
  return true;
}
//弹出一个元素:
//成功as_stat = as_ok, 返回true
//失败as_stat = as_err_stack_empty, 返回false
bool _array_stack::as_pop(as_element_type *ret_val){
  struct as_info* pas = &this->as;

  if(!this->as_check_empty())
    return false;//栈空检查失败

  *ret_val = pas->as_buf[pas->as_cur_size];
  pas->as_buf[pas->as_cur_size] = aq_err_val_init;
  pas->as_cur_size--;
  pas->as_stat = as_ok;
  return true;
}
//
//返回栈的当前的元素个数
void _array_stack::as_get_cur_size(int *cur_size){
  struct as_info* pas = &this->as;
  *cur_size = pas->as_cur_size+1;
}
//压栈N个元素(不支持最大限度插入):
//成功as_stat = as_ok, 返回true
//失败as_stat = as_err_stack_full, 返回false
bool _array_stack::as_pushN(as_element_type *val_arr, int N){
  assert(val_arr != NULL);//必须保证指针不为空, 否则程序就应该终止了
  struct as_info* pas = &this->as;
  //插入安检
  int i = 0;
  for(;i < N;i++){
    if( (val_arr[i] <= as_max_val_low) && (val_arr[i] >= as_max_val_high) ){
      pas->as_stat = as_err_val_overflow;
      pas->as_test_err_count++;
      return false;//只要有一个值是错误的, 就不执行插入
    }
  }

  if(!this->as_check_full())
    return false;//栈满检查失败

  int tmp,tmp2;
  if(pas->as_cur_size == as_sig_pos_empty){
    tmp = N;//栈为空时, as_cur_size = -1...所以tmp 直接等于多少个元素(判断批量插入是否满栈溢出)
    tmp2 = 0;
  }
  else{
    tmp = pas->as_cur_size + N;
    tmp2 = pas->as_cur_size;
  }

  if(tmp > as_sig_pos_full){//防止该次批量插入的量大于栈当前的容纳量
    //不支持尽力插入...超出最大栈容量就返回插入错误, 你也无法得知具体是哪儿错误
    //刚好等于as_sig_pos_full 就兼容插入
    pas->as_stat = as_err_stack_full;
    pas->as_test_err_count++;
    return false;
  }

  //通过批量插入条件, 执行正常插入
  i = 0;
  for(;tmp2 < tmp;tmp2++){
    pas->as_buf[tmp2] = val_arr[i];
    i++;
  }
  pas->as_cur_size = tmp;//更改栈当前容量
  pas->as_stat = as_ok;
  return true;
}
//弹出N个元素[N<=0, 提空栈]:
//成功as_stat = as_ok, 返回true
//失败as_stat = as_err_stack_empty, 返回false
bool _array_stack::as_popN(as_element_type *re_val_arr, int N){
  assert(re_val_arr != NULL);//必须保证指针不为空, 否则程序就应该终止了
  struct as_info* pas = &this->as;

  if(!this->as_check_empty())
    return false;//栈空检查失败

  if(N <= 0)//N <= 0, 提空栈
    N = pas->as_cur_size;

  if(N > pas->as_cur_size){//栈没有那么多元素
    pas->as_stat = as_err_stack_poor;
    N = pas->as_cur_size;
  }

  //批量提取, 直接内存CP, 但是这个操作很危险!!
  //务必保证传入的指针指向的缓冲区容量 == N, 两个数值必须一致
  //我没办法直接检查缓冲区的容量
  memcpy(re_val_arr, pas->as_buf, N*(sizeof(as_element_type)));
  memset(pas->as_buf, aq_err_val_init, N*(sizeof(as_element_type)));
  pas->as_cur_size = pas->as_cur_size - N;//更改栈当前容量
  pas->as_stat = as_ok;
  return true;
}
//
//检查栈有没有重大错误
void _array_stack::as_check(int *cur_err_count){
  struct as_info* pas = &this->as;
  *cur_err_count = pas->as_test_err_count;
}
//
//if 空栈
bool _array_stack::as_check_empty(void){
  //判断是否栈空
  struct as_info* pas = &this->as;
  if(pas->as_cur_size <= as_sig_pos_empty){
    pas->as_stat = as_err_stack_empty;//栈空
    if(pas->as_cur_size != as_sig_pos_empty){
      pas->as_stat = as_err_stack_emptyEx;//栈空且溢出(错误)
      pas->as_test_err_count++;
      pas->as_cur_size = as_sig_pos_empty;//丢值, 修改
    }
    return false;
  }
  if(pas->as_cur_size > as_sig_pos_full){
    pas->as_stat = as_err_exception;
    pas->as_test_err_count++;
    pas->as_cur_size = as_sig_pos_full;//丢值, 修改
    return false;
  }
  return true;
}
//
//if 满栈
bool _array_stack::as_check_full(void){
  //判断是否栈满
  struct as_info* pas = &this->as;
  if(pas->as_cur_size >= as_sig_pos_full){
    pas->as_stat = as_err_stack_full;//栈满
    if(pas->as_cur_size != as_sig_pos_full){
      pas->as_stat = as_err_stack_fullEx;//栈满且溢出(错误)
      pas->as_test_err_count++;
      pas->as_cur_size = as_sig_pos_full;//丢值, 修改
    }
    return false;
  }
  if(pas->as_cur_size < as_sig_pos_empty){
    pas->as_stat = as_err_exception;
    pas->as_test_err_count++;
    pas->as_cur_size = as_sig_pos_empty;//丢值, 修改
    return false;
  }
  return true;
}
//
//模块测试函数
void _array_stack::as_test(void){
  printf("************************************\n");
  printf("array stack test start\n");
  printf("************************************\n");

  int test_err = 0;
  this->as_reset();//初始化栈
  struct as_info *pas = &this->as;

  //获取栈当前容量=as_buf_empty 测试
  int cur_size = -100;
  this->as_get_cur_size(&cur_size);
  if(cur_size != 0){
    printf("array stack get cur_size=as_buf_empty test fail\n");
    test_err++;
  }

  //空栈pop 测试

  as_element_type as_pop_x = 999;
  bool bb = this->as_pop(&as_pop_x);
  if(!(pas->as_stat == as_err_stack_empty && pas->as_cur_size == as_sig_pos_empty)){
    printf("array stack pop empty-stack test fail\n");
    test_err++;
  }

  //压入一个值测试
  as_pop_x = 999;
  if(!(this->as_push(as_pop_x) && pas->as_stat == as_ok && pas->as_buf[0] == as_pop_x)){
    printf("array stack push one test fail\n");
    test_err++;
  }

  //pop 空测试
  as_pop_x = 0;
  if(!(this->as_pop(&as_pop_x) && pas->as_stat == as_ok)){
    printf("array stack pop empty-stack test fail\n");
    test_err++;
  }

  //将栈填满 测试--填不满就有容量缺陷
  int full_test;
  for(full_test = 0;full_test < as_max_buf;full_test++){
    bb = this->as_push(full_test);
    if(!(bb && pas->as_stat == as_ok && pas->as_buf[full_test] == full_test)){
      printf("array stack full-all test fail\n");
      test_err++;
      printf("********%d\n",full_test);
      break;
    }
  }

  //获取栈当前容量=as_max_buf 测试
  cur_size = -100;
  this->as_get_cur_size(&cur_size);
  if(cur_size != as_max_buf){
    printf("array stack get cur_size=as_max_buf test fail\n");
    test_err++;
  }
  printf("xxxxx_now stack size = %d\n", cur_size);

  //满栈push 测试
  bb = this->as_push(1000);
  if(!((!bb) && pas->as_stat == as_err_stack_full)){
    printf("array stack push full and one test fail\n");
    test_err++;
  }

  //将填满后的栈掏空
  int pop_test_count = as_max_buf - 1;
  for(full_test = pop_test_count;full_test >= 0;full_test--){
    as_pop_x = 0;
    if(!(this->as_pop(&as_pop_x) && pas->as_stat == as_ok)){
      printf("array stack pop a full-stack test fail\n");
      test_err++;
      printf("********%d\n",full_test);
    }
  }

  //pop 空测试
  as_pop_x = 0;
  bb = this->as_pop(&as_pop_x);
  if(!((!bb) && pas->as_stat == as_err_stack_empty)){
    printf("array stack pop a full-stack and one test fail\n");
    test_err++;
  }

  //获取栈当前容量=as_max_buf 测试
  cur_size = -100;
  this->as_get_cur_size(&cur_size);
  if(cur_size != 0){
    printf("*****array stack get cur_size=0 test fail\n");
    test_err++;
  }
  printf("xxxxx_now stack size = %d\n", cur_size);

  //栈系统正常...这时候测试批量push 入是否正常...也方便提供使用案例
  int xx = 0;
  as_element_type as_arr[999];
  for(;xx < 999;xx++)
    as_arr[xx] = xx;
  if(!this->as_pushN(as_arr,999) && pas->as_stat == as_ok){
    printf("as_max_buf_pushN full-all tmp buf fail !!, now we full in %d times",xx);
    test_err++;
  }

  if(!(pas->as_buf[0] == 0 && pas->as_buf[998] == 999 && pas->as_buf[1] == 1)){
    printf("批量插入值正确\n");
  }

  memset(as_arr,0,sizeof(as_element_type)*999);

  if(!this->as_popN(as_arr,999) && pas->as_stat == as_ok){
    printf("as_max_buf_pushN full-all tmp buf fail !!, now we full in %d times",xx);
    test_err++;
  }

  if(!(as_arr[0] == 0 && as_arr[998] == 999 && as_arr[1] == 1)){
    printf("批量弹出值正确\n");
  }

  //测试汇总
  if(test_err == 0)
    printf("********************array stack test ok************************\n");
  else
	printf("********************array stack test fail, please check it out why ************************\n");
  printf("important error notes print now: stack = %d\n", pas->as_test_err_count);

  return;
}


