/*
 * _array_stack.h
 *
 *  Created on: Aug 16, 2017
 *      Author: adan
 */

#include <stdio.h>
//*********************
//运行宏
//*********************
#define INT_MAX (2147483647 - 17)
#define INT_MIN (-INT_MAX - 1)

#define as_element_type int//4个字节(固定)
#define as_max_buf 40960
//#define as_max_buf_pushN 8192//批量插入时,你应确保不超过max_buf_pushN,只允许良性插入,尽量将错误挡在上一层
#define as_sig_pos_empty -1 //缓冲区空 -1 (由于堆栈比较特殊所以...)
#define as_sig_pos_full (as_max_buf - 1)//缓冲区满 as_buf_max - 1

//值域
#define as_max_val_high INT_MAX
#define as_max_val_low INT_MIN
//错误标记定义
#define as_err_val_overflow (INT_MAX+1) //值溢出
#define as_err_max_val_high (INT_MAX+2) //高位溢出
#define as_err_max_val_low (INT_MAX+3) //低位溢出
#define as_err_stack_full (INT_MAX+4) //栈满
#define as_err_stack_empty (INT_MAX+5) //栈空
#define as_err_stack_fullEx (INT_MAX+6) //栈满且溢出错误
#define as_err_stack_emptyEx (INT_MAX+7) //栈空且溢出错误
#define as_err_stack_poor (INT_MAX+8) //(栈里面并没有那么多元素, 这个错误操作被兼容为: 提空栈)
#define as_err_exception (INT_MAX+9) //检查空栈时却栈溢出/检查栈满时却栈溢出
#define as_ok (INT_MAX+10) //操作ok
#define aq_err_val_init (INT_MAX+12) //初始化值--pop 出后还原
//错误字符串定义
#define as_err_msg_stack_full "stack full" //栈满
#define as_err_msg_stack_empty "stack empty" //栈空
#define as_err_msg_val_overflow "push a overflow value" //插入值溢出
#define as_err_msg_len 32 //就只有栈满, 栈空两条


//*********************
//运行结构体
//*********************
struct as_info{
  as_element_type as_buf[as_max_buf];//栈缓冲区
  //as_element_type as_buf_pushN[as_max_buf_pushN];//批量插入缓冲区
  //为了不增加程序复杂性,插入错误统一返回栈满,每次批量插入不能大于<as_max_buf_pushN>
  //你不能返回已经插入了多少个元素, 所以还是{不能支持<最大限度插入>}比较好...不可能的
  //(批量导出是没有需求的)

  int as_cur_size;//栈当前容量
  int as_stat;//栈当前状态(反正是不具备多线程操作的, 出错信息全部修改这里)

  int as_test_err_count;//堆栈溢出cache...发现这个不为0则队列出现过巨大的错误

//附：栈中不能存在-负值元素<69999997 -- 实测
//+-4294967296(c 语言int-32bit 极限是42ww)
};

//基于环形数组的栈
class _array_stack{
public:
  //*********************
  //运行变量
  //*********************
  struct as_info as;
  //*********************
  //类方法
  //*********************
  //容器as_info 初始化
  void as_reset(void);
  //压栈一个元素(成功返回as_ok, 失败as_stat=as_err_stack_full)
  bool as_push(as_element_type val);
  //弹出一个元素(成功返回栈首的值, 失败as_stat=as_err_stack_empty)
  bool as_pop(as_element_type *ret_val);
  //返回栈的当前的元素个数(成功返回当前数量, 不会失败)
  void as_get_cur_size(int *cur_size);
  //压栈N个元素(成功as_stat=as_ok, 失败as_stat=as_err_stack_full)
  bool as_pushN(as_element_type *val_arr, int N);
  //弹出N个元素[N<=0, 提空栈](成功as_stat=as_ok, 失败as_stat=as_err_stack_empty)
  bool as_popN(as_element_type *re_val_arr, int N);
  //获取错误字符串
  void as_get_err_msg(char *err_msg_buf);

  _array_stack(void);//构造函数(不需要声明返回类型, 构造函数默认没有返回)
  ~_array_stack(void);//释构函数
private:
  bool as_check_empty(void);//if 空栈
  bool as_check_full(void);//if 满栈
  void as_check(int *cur_err_count);//获取栈的错误计数
  void as_test(void);//模块测试函数(外人不准用)
};
