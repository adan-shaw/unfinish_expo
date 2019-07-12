/*
 * _array_queue.h
 *
 *  Created on: Aug 16, 2017
 *      Author: adan
 */

#include <stdio.h>
#include <pthread.h>
//*********************
//运行宏
//*********************
#define INT_MAX (2147483647 - 17)
#define INT_MIN (-INT_MAX - 1)

#define aq_element_type int//4个字节(固定)
#define aq_max_buf 40960//for test, 测试压满, 你可以随意修改容器大小, 支持容器大小动态修改
//
//#define aq_max_buf_pushN 8192//批量插入时,你应确保不超过max_buf_pushN,只允许良性插入,尽量将错误挡在上一层
#define aq_sig_pos_empty 0 //缓冲区空
#define aq_sig_pos_full (aq_max_buf - 1)//缓冲区满 aq_buf_max - 1

//值域
#define aq_max_val_high INT_MAX
#define aq_max_val_low INT_MIN
//错误标记定义(多线程访问, 标记根本没用, 很快会被切换的)
//但线程有用, 调试的时候还是有用的...
#define aq_err_val_overflow (INT_MAX+1) //值溢出
#define aq_err_max_val_high (INT_MAX+2) //高位溢出
#define aq_err_max_val_low (INT_MAX+3) //低位溢出
#define aq_err_queue_full (INT_MAX+4) //队列满
#define aq_err_queue_empty (INT_MAX+5) //队列空
#define aq_err_queue_fullEx (INT_MAX+6) //队列满且溢出错误
#define aq_err_queue_emptyEx (INT_MAX+7) //队列空且溢出错误
#define aq_err_queue_poor (INT_MAX+8) //(队列里面并没有那么多元素, 这个错误操作被兼容为: 提空队列)
#define aq_err_exception (INT_MAX+9) //检查空队列时却队列溢出/检查队列满时却队列溢出
//#define aq_err_frontRear_curSize (INT_MAX+11) //
#define aq_err_val_init (INT_MAX+12) //初始化值
#define aq_ok (INT_MAX+10) //操作ok
//错误字符串定义
#define aq_err_msg_queue_full "queue full" //队列满
#define aq_err_msg_queue_empty "queue empty" //队列空
#define aq_err_msg_val_overflow "push a overflow value" //插入值溢出
#define aq_err_msg_len 32 //就只有队列满, 队列空两条


//*********************
//运行结构体
//*********************
struct aq_info{
  aq_element_type aq_buf[aq_max_buf];//队列缓冲区
  //aq_element_type aq_buf_pushN[aq_max_buf_pushN];//批量插入缓冲区
  //为了不增加程序复杂性,插入错误统一返回队列满,每次批量插入不能大于<aq_max_buf_pushN>
  //你不能返回已经插入了多少个元素, 所以还是{不能支持<最大限度插入>}比较好...不可能的
  //(批量导出是没有需求的)

  int aq_cur_pos_front;//队列头--活动的, 不确定具体的pos
  int aq_cur_pos_rear;//队列尾
  int aq_cur_size;//队列当前容量
  int aq_stat;//队列当前状态(反正是不具备多线程操作的, 出错信息全部修改这里)

  int aq_test_err_count;//堆队列溢出cache...发现这个不为0则队列出现过巨大的错误
//附：队列中不能存在-负值元素<69999997 -- 实测
//+-4294967296(c 语言int-32bit 极限是42ww)

  pthread_rwlock_t aq_rwlock;
};

//基于环形数组的队列
class _array_queue{
public:
  //*********************
  //运行变量
  //*********************
  struct aq_info aq;
  //*********************
  //类方法
  //*********************
  //容器aq_info 初始化
  void aq_reset(void);
  //压队列一个元素(成功返回aq_ok, 失败aq_stat=aq_err_queue_full)
  bool aq_push(aq_element_type val, bool push_rear);
  //弹出一个元素(成功返回队列首的值, 失败aq_stat=aq_err_queue_empty)
  bool aq_pop(aq_element_type *ret_val, bool pop_front);
  //返回队列的当前的元素个数(成功返回当前数量, 不会失败)
  void aq_get_cur_size(int *cur_size);
  //压队列N个元素(成功aq_stat=aq_ok, 失败aq_stat=aq_err_queue_full)
  bool aq_pushN(aq_element_type *val_arr, int N, bool push_rear);
  //弹出N个元素[N<=0, 提空队列](成功aq_stat=aq_ok, 失败aq_stat=aq_err_queue_empty)
  bool aq_popN(aq_element_type *re_val_arr, int N, bool pop_front);
  //获取错误字符串
  void aq_get_err_msg(char *err_msg_buf);

  _array_queue(void);//构造函数(不需要声明返回类型, 构造函数默认没有返回)
  ~_array_queue(void);//释构函数
private:
  bool aq_check_empty(int *ret_cur_size);//if 空队列
  bool aq_check_full(int N);//if 满队列
  //移动游标, 执行push
  void movePos_doPush(aq_element_type *val_arr, int N, bool push_rear);
  //移动游标, 执行pop
  void movePos_doPop(aq_element_type *re_val_arr, int N, bool pop_front);
  void aq_check(int *cur_err_count);//获取队列的错误计数
  void aq_test(void);//模块测试函数(外人不准用)
};
