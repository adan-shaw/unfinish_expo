/*
 * array_queue.h
 *
 *  Created on: Nov 13, 2016
 *      Author: adan
 */

//************
//环形数组-队列(魔橙互联说这还不算环形数组--暂时叫:改造数组-队列)
//************
#include <pthread.h>

//******
//运行宏
//******
#define type_element int//4个字节(固定)
#define aq_max_buf 40960
#define aq_max_tmpbuf 1024//4kb

//************
//运行信息体实体
//************
struct aq_info{
  type_element aq_buf[aq_max_buf];//队列缓冲区
  int aq_cur_pos_front;//队列头--活动的, 不确定具体的pos
  int aq_cur_pos_rear;//队列尾
  int aq_cur_size;//队列当前容量

  pthread_mutex_t aq_mutex_tmp;//自带锁--唯一
  type_element aq_tmpbuf_push[aq_max_tmpbuf];//压入临时缓冲区-功能：载体--唯一
  type_element aq_tmpbuf_pop[aq_max_tmpbuf];//pop出临时缓冲区-功能：载体--唯一

  int aq_sig_turn_pos;//array 调头重来的标志数
  int aq_sig_full;//array 满标志--also a 压入失败标志
  int aq_sig_empty;//array 空标志--also a pop 出失败标志
  int aq_sig_push_ok;//压入ok 标志...by the way, pop 出ok 时返回pop 出的数量
  int aq_sig_msg_err;//队列溢出cache...发现这个不为0则队列出现过巨大的错误(整数是溢出装载, 负数是刨空)
};

//***********
//函数前置声明
//***********
//容器as_info 初始化
void aq_init(struct aq_info* paq);
//压栈一个元素(成功返回as_sig_push_ok, 失败返回as_sig_full)
int aq_pushN(struct aq_info* paq, int N);
//弹出一个元素(成功返回 >= -4000000000 的值, 失败返回as_sig_empty)
type_element aq_popN(struct aq_info* paq);
//返回栈的当前的元素个数(成功返回当前数量, 不会失败)
int aq_get_size(struct aq_info* paq);
//检查栈有没有重大错误(没错误返回0, 有错误返回-1, 不会失败)
int aq_check(struct aq_info* paq);

//-- 判断为空  -if(aq_get_size() == 0)
//-- if full? -if(aq_get_size() == aq_max_buf)

//模块自测函数
void aq_test(void);
