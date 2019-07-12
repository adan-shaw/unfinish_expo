/*
 * array_stack.h
 *
 *  Created on: Nov 13, 2016
 *      Author: adan
 */

//**********
//环形数组-栈(魔橙互联说这还不算环形数组--暂时叫:改造数组-栈)
//**********

//******
//运行宏
//******
#define type_element int//4个字节(固定)
#define as_max_buf 40960
#define as_max_buf_pushN 1024//批量插入时,你应确保不超过max_buf_pushN,只允许良性插入,尽量将错误挡在上一层

//************
//运行信息体实体
//************
struct as_info{
  type_element as_buf[as_max_buf];//栈缓冲区
  int as_cur_size;//栈当前容量

  int as_pos_empty;//缓冲区空 -1 (由于堆栈比较特殊所以...)
  int as_pos_full;//缓冲区满 as_buf_max - 1

  int as_sig_full;//满标志--also a 压入失败标志
  int as_sig_empty;//空标志--also a pop 出失败标志
  int as_sig_push_ok;//压入标志
  int as_sig_msg_err;//堆栈溢出cache...发现这个不为0则队列出现过巨大的错误

  type_element as_buf_pushN[as_max_buf_pushN];//批量插入缓冲区


  /* 为了不增加程序复杂性,插入错误统一返回栈满,所以必须保证栈的容量必须大于服务器的最大在线用户量,40960 应该可以了*/
  //int as_sig_pushN_err;//批量插入错误--可能只是正常栈满, 也有可能是不能插入了, 也可能是栈错误了...
  //int as_sig_pushN_xerr;//栈溢出-损坏
  //int as_sig_pushN_xerr2;//此次插入的量比栈本身的容量大
  //int as_sig_pushN_xerr3;//此次插入的量比栈本身的容量大, 但最大限度插入了n 个元素??
  //你不能返回已经插入了多少个元素, 所以这里还是不能支持<最大限度插入>...不可能的

  //(批量导出是没有需求的...)
};

//***********
//函数前置声明
//***********
//容器as_info 初始化
void as_init(struct as_info* pas);
//压栈一个元素(成功返回as_sig_push_ok, 失败返回as_sig_full)
int as_push(struct as_info* pas, type_element val);
//弹出一个元素(成功返回 >= -4000000000 的值, 失败返回as_sig_empty)
type_element as_pop(struct as_info* pas);
//返回栈的当前的元素个数(成功返回当前数量, 不会失败)
int as_get_size(struct as_info* pas);
//压栈N个元素(成功返回as_sig_push_ok, 失败返回as_sig_full)
int as_pushN(struct as_info* pas, int N);
//检查栈有没有重大错误(没错误返回0, 有错误返回-1, 不会失败)
int as_check(struct as_info* pas);
//模块自测函数
void as_test(void);

