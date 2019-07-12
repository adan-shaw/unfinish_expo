/*
 * array_stack.c
 *
 *  Created on: Nov 13, 2016
 *      Author: adan
 */

#include "array_stack.h"

//初始化函数-反正只调用一次,虽然只有一句还是为重用考虑吧
//附：栈中不能存在-负值元素<69999997 -- +-4294967296(c 语言int-32bit 极限是42ww)
void as_init(struct as_info* pas){
  pas->as_cur_size = -1;//栈比较特殊,这里不能为0, 设计为-1 可以减少一个if 的使用
  pas->as_pos_empty = -1;
  pas->as_pos_full = as_max_buf -1;
  pas->as_sig_full = -4000000000;//经过测试, 这个值算是最稳定的...
  pas->as_sig_empty = -4000000001;
  pas->as_sig_push_ok = -4000000002;
  pas->as_sig_msg_err = 0;

  //pas->as_sig_pushN_err = -4000000003;
  //pas->as_sig_pushN_xerr = -4000000004;
  return;
}
//压栈一个-成功返回as_sig_push_ok
//实际场景用不上多个push 入,因为stack 就是用来收集epoll fd 后再全部倒出
int as_push(struct as_info* pas, type_element val){
  //判断是否栈满
  if(pas->as_cur_size >= pas->as_pos_full){
	if(pas->as_cur_size > pas->as_pos_full){
	  pas->as_sig_msg_err = pas->as_cur_size;
	  pas->as_cur_size = pas->as_pos_full;
	  printf("array stack current volume > as_pos_full...it's a very important error !!\n");
	}
	else
      printf("array stack has full from as_push\n");
    return pas->as_sig_full;
  }
  pas->as_cur_size++;
  pas->as_buf[pas->as_cur_size] = val;
  return pas->as_sig_push_ok;
}
//弹出一个-成功返回 >=0 的值
//实际场景也用不上全部倒出，全部倒出还不如直接COPY 整个stack-缓冲区到新的栈中慢慢操作
type_element as_pop(struct as_info* pas){
  //判断是否栈空--不要做胆小的程序员, 当然在栈为空时重置栈会更安全, 但是这是消耗性能的
  if(pas->as_cur_size <= pas->as_pos_empty){
    if(pas->as_cur_size != pas->as_pos_empty){
	  pas->as_sig_msg_err = pas->as_cur_size;
	  pas->as_cur_size = pas->as_pos_empty;
	  printf("array stack current volume != as_pos_empty...it's a very important error !!\n");
	}
    else
      printf("array stack is empty from as_pop\n");
    return pas->as_sig_empty;
  }
  type_element tmp = pas->as_buf[pas->as_cur_size];
  pas->as_cur_size--;
  return tmp;
}
//
//返回栈的当前的元素个数
int as_get_size(struct as_info* pas){
  if(pas->as_cur_size <= pas->as_pos_empty)
    return 0;
  return pas->as_cur_size+1;
}
//
//压栈N个-成功返回as_sig_push_ok, 失败统一返回as_sig_full...不支持最大限度插入...
//为了支持最大限度插入, 又能兼容多种错误返回, 这里错误统一判断方式是: tmp_return < as_sig_pushN_err
int as_pushN(struct as_info* pas,int N){
  //判断是否栈满
  if(pas->as_cur_size >= pas->as_pos_full){
    if(pas->as_cur_size == pas->as_pos_full){
	  printf("array stack has full from as_pushN\n");
      return pas->as_sig_full;
    }
    else{
	  printf("array stack current volume > as_pos_full...it's a very important error !!\n");
	  pas->as_sig_msg_err = pas->as_cur_size;
	  pas->as_cur_size = pas->as_pos_full;
	  return pas->as_sig_full;//栈坏--但仍然返回as_sig_full--这种情况不太可能有, 放心吧
    }

  }

  int tmp,tmp2;
  if(pas->as_cur_size == pas->as_pos_empty){
    tmp = N;//栈为空时，as_cur_size = -1...所以tmp 直接等于多少个元素(判断批量插入是否满栈溢出)
    tmp2 = 0;
  }
  else{
	tmp = pas->as_cur_size + N;
	tmp2 = pas->as_cur_size;
  }

  if(tmp > pas->as_pos_full){//防止该次批量插入的量大于栈当前的容纳量
	//不支持尽力插入...超出最大栈容量就返回插入错误, 你也无法得知具体是哪儿错误
	//刚好等于pas->as_pos_full 就兼容插入
    return pas->as_sig_full;
  }

  //通过批量插入条件, 正常插入
  int n;
  for(n = 0;tmp2 < tmp;tmp2++){
    pas->as_buf[tmp2] = pas->as_buf_pushN[n];
    n++;
  }
  pas->as_cur_size = tmp;//更改栈当前容量
  return pas->as_sig_push_ok;
}
//
//检查栈有没有重大错误
int as_check(struct as_info* pas){
  if(pas->as_sig_msg_err == 0)
	return 0;
  else
	return -1;
}
//
//模块自测函数
void as_test(void){
  printf("************************************\n");
  printf("array stack test start\n");
  printf("************************************\n");
  int test_err = 0;
  struct as_info as_test;
  //初始化栈
  as_init(&as_test);
  //获取栈当前容量=as_buf_empty 测试
  if(as_get_size(&as_test) != 0){
    printf("array stack get cur_size=as_buf_empty test fail\n");
    test_err++;
  }
  //空栈pop 测试
  if(as_pop(&as_test) != as_test.as_sig_empty){
    printf("array stack pop empty-stack test fail\n");
    test_err++;
  }
  //将栈填满 测试--填不满就有容量缺陷
  int full_test;
  for(full_test = 0;full_test < as_max_buf;full_test++){
    if(as_push(&as_test,full_test) != as_test.as_sig_push_ok){//不等于as_sig_push_ok 就是push 错
	  printf("array stack full-all test fail\n");
	  test_err++;
	  printf("********%d\n",full_test);
	  break;
	}
  }
  //获取栈当前容量=as_max_buf 测试
  if(as_get_size(&as_test) != as_max_buf){
    printf("array stack get cur_size=as_max_buf test fail\n");
    test_err++;
  }
  //满栈push 测试
  if(as_push(&as_test,10000) != as_test.as_sig_full){
    printf("array stack push full-stack test fail\n");
    test_err++;
  }
  //将填满后的栈掏空
  int pop_test_count = as_max_buf - 1;
  for(full_test = pop_test_count;full_test < 0;full_test--){
	int xtmp = as_pop(&as_test);
    if(xtmp == as_test.as_sig_empty){
	  printf("array stack pop a full-stack test fail\n");
	  test_err++;
	  printf("********%d\n",full_test);
	  break;
    }
  }
  //栈系统正常...这时候测试批量push 入是否正常...也方便提供使用案例
  int xx = 0;
  for(;xx < as_max_buf_pushN;xx++){
	as_test.as_buf_pushN[xx] = xx;//将批量插入缓冲区写满
  }
  if(xx != as_max_buf_pushN){
	printf("as_max_buf_pushN full-all tmp buf fail !!, now we full in %d times",xx);
	test_err++;
  }
  //执行批量插入
  xx = as_pushN(&as_test,xx);
  if(xx != as_test.as_sig_full){
	printf("as_max_buf_pushN once fail !!");
	test_err++;
  }
  //批量插入错误触发
  int xxx = xx;
  for(;xx<=as_max_buf;xx=xx+xxx){
	xxx = as_pushN(&as_test,xx);
	if(xx != as_max_buf_pushN){//只是每次插入批量缓冲区的最大容量而已
	  if(xx != as_max_buf){//如果批量插入溢出时不等于极限
	    printf("as_max_buf_pushN full-all buf fail !!, now we full in %d times",xx);
	    test_err++;
	  }
	}
  }
  //
  if(test_err == 0)
    printf("********************array stack test ok************************\n");
  else
	printf("********************array stack test fail, please check it out why ************************\n");
  printf("important error notes print now: stack = %d\n", as_test.as_sig_msg_err);
  return;
}
