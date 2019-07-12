/*
 * array_queue.c
 *
 *  Created on: Nov 13, 2016
 *      Author: adan
 */

#include "array_queue.h"

//初始化array queue
//附：栈中不能存在-负值元素<69999997 -- +-4294967296(c 语言int-32bit 极限是42ww)
void aq_init(struct aq_info* paq){
  paq->aq_cur_pos_front = 0;
  paq->aq_cur_pos_rear = 0;
  paq->aq_cur_size = 0;
  pthread_mutex_init(&paq->aq_mutex_tmp,NULL);

  paq->aq_sig_turn_pos = aq_max_buf - 1;
  paq->aq_sig_full = -4000000000;//经过测试, 这个值算是最稳定的...
  paq->aq_sig_empty = -4000000001;
  paq->aq_sig_push_ok = -4000000002;
  paq->aq_sig_msg_err = 0;
  return;
}
//
//获取array queue 的cur_size
//-- 判断为空  -if(aq_get_size() == 0)
//-- if full? -if(aq_get_size() == aq_max_buf)
int aq_get_size(struct aq_info* paq){
  int tmp;
  pthread_mutex_lock(&paq->aq_mutex_tmp);
  tmp = paq->aq_cur_size;
  pthread_mutex_unlock(&paq->aq_mutex_tmp);
  return tmp;
}
//
//压入数据pushN
int aq_pushN(struct aq_info* paq,int N){
  int tmp;
  int D_val;
  pthread_mutex_lock(&paq->aq_mutex_tmp);
  if(paq->aq_cur_size >= aq_max_buf){
	if(paq->aq_cur_size > aq_max_buf){
	  paq->aq_sig_msg_err = paq->aq_cur_size;//记录致命错误
	  paq->aq_cur_size = aq_max_buf;
	  printf("array queue current volume > aq_mutex_tmp...it's a very important error !!\n");
	}
	else
	  printf("array queue has full\n");
    pthread_mutex_unlock(&paq->aq_mutex_tmp);
    return paq->aq_sig_full;
  }
  D_val = paq->aq_cur_pos_rear + N;//PUSH 入之后的游标是否溢出
  if(D_val >= aq_max_buf){//调头
    D_val = D_val - aq_max_buf;//求得溢出容量差
    for(tmp = 0;paq->aq_cur_pos_rear <= paq->aq_sig_turn_pos;paq->aq_cur_pos_rear++){//未溢出部分继续加
      paq->aq_buf[paq->aq_cur_pos_rear] = paq->aq_tmpbuf_push[tmp];
      tmp++;
    }
    for(paq->aq_cur_pos_rear = 0;paq->aq_cur_pos_rear < D_val;paq->aq_cur_pos_rear++){//溢出移到队首
      paq->aq_buf[paq->aq_cur_pos_rear] = paq->aq_tmpbuf_push[tmp];
      tmp++;
    }
    paq->aq_cur_size = paq->aq_cur_size + N;//总数+N
  }
  else{//不用调头
    for(tmp = 0;paq->aq_cur_pos_rear < D_val;paq->aq_cur_pos_rear++){//直接插入
      paq->aq_buf[paq->aq_cur_pos_rear] = paq->aq_tmpbuf_push[tmp];
      tmp++;
    }
    paq->aq_cur_size = paq->aq_cur_size + N;;//总数+N
  }
  //执行插入操作--不能离开缓冲区，因为当你push 内存操作时，内存还没写入，但是就已经pop 出的时候
  //就会发生错误，所以这点性能还是不能省
  pthread_mutex_unlock(&paq->aq_mutex_tmp);
  return paq->aq_sig_push_ok;
}
//
//弹出数据，尽量弹出全部，但是最多弹出4096，返回弹出的数量
int aq_popN(struct aq_info* paq){
  int tmp;
  int D_val;
  int pop_count;
  pthread_mutex_lock(&paq->aq_mutex_tmp);
  if(paq->aq_cur_size <= 0){
    if(paq->aq_cur_size < 0){
	  paq->aq_sig_msg_err = paq->aq_cur_size;//记录致命错误
	  paq->aq_cur_size = 0;
	  printf("array queue current volume < 0...it's a very important error !!\n");
	}
    else
      printf("array queue is empty\n");
    pthread_mutex_unlock(&paq->aq_mutex_tmp);
    return paq->aq_sig_empty;
  }
  pop_count = paq->aq_cur_size;	//获取当前队列容量
  if(pop_count > aq_max_tmpbuf)	//队列当前任务的数量大于缓冲区大小时
    pop_count = aq_max_tmpbuf;
  D_val = paq->aq_cur_pos_front + pop_count; //POP 入之后的游标是否溢出
  if(D_val >= aq_max_buf){//调头
    D_val = D_val - aq_max_buf;	//求得溢出容量差
    for(tmp = 0;paq->aq_cur_pos_front <= paq->aq_sig_turn_pos;paq->aq_cur_pos_front++){//未溢出部分继续加
      paq->aq_tmpbuf_pop[tmp] = paq->aq_buf[paq->aq_cur_pos_front];
      tmp++;
    }
    for(paq->aq_cur_pos_front = 0;paq->aq_cur_pos_front < D_val;paq->aq_cur_pos_front++){//溢出移到队首
      paq->aq_tmpbuf_pop[tmp] = paq->aq_buf[paq->aq_cur_pos_front];
      tmp++;
    }
    paq->aq_cur_size = paq->aq_cur_size - pop_count;	//总数-pop_count
  }
  else{//不用调头
    for(tmp = 0;paq->aq_cur_pos_front < D_val;paq->aq_cur_pos_front++){//直接弹出
      paq->aq_tmpbuf_pop[tmp] = paq->aq_buf[paq->aq_cur_pos_front];
      tmp++;
    }
    paq->aq_cur_size = paq->aq_cur_size - pop_count;	//总数-pop_count
  }
  return pop_count;
}
//
//检查栈有没有重大错误
int aq_check(struct aq_info* paq){
  if(paq->aq_sig_msg_err == 0)
	return 0;
  else
	return -1;
}
//
//模块自测函数
void aq_test(void){
  printf("************************************\n");
  printf("array queue test start\n");
  printf("************************************\n");
  int test_err = 0;
  struct aq_info aq_test;
  aq_init(&aq_test);
  //pop 出空栈测试
  if(aq_popN(&aq_test) != aq_test.aq_sig_empty){
    printf("array queue pop empty test fail\n");
    test_err++;
  }
  //空队列判断测试
  if(aq_get_size(&aq_test) != 0){
    printf("array queue get cur_size=0 test fail\n");
    test_err++;
  }
  //压满队列测试
  int tmp2;
  for(tmp2 = 0;tmp2 < aq_max_tmpbuf;tmp2++){
    aq_test.aq_tmpbuf_push[tmp2] = tmp2;//对缓冲区写满数据
  }
  for(tmp2 = 0;tmp2 < aq_max_buf/aq_max_tmpbuf;tmp2++){//执行压满
    if(aq_pushN(&aq_test,aq_max_tmpbuf) != aq_test.aq_sig_push_ok){
	  printf("array queue push element till full test fail\n");
	  test_err++;
    }
  }
  //判断是否队满测试
  if(aq_get_size(&aq_test) != aq_max_buf){
    printf("push full array queue test fail\n");
    test_err++;
  }
  //满队列时仍然要压入的测试
  if(aq_pushN(&aq_test,aq_max_tmpbuf) != aq_test.aq_sig_full){
    printf("array queue push full test fail\n");
    test_err++;
  }
  //对存入的数据进行测试
  if(aq_test.aq_buf[0] != 0){
    printf("array queue push element value is right ? test fail\n");
    test_err++;
    printf("now aq_buf[0]=%d\n",aq_test.aq_buf[0]);
  }
  if(aq_test.aq_buf[aq_max_tmpbuf-1] != aq_max_tmpbuf-1){//0-1023, 1024 = 0
    printf("array queue push element value is right ? test fail\n");
    test_err++;
    printf("now aq_buf[aq_max_tmpbuf-1]=%d-%d\n",aq_test.aq_buf[aq_max_tmpbuf-1],aq_max_tmpbuf-1);
  }
  if(aq_test.aq_buf[aq_max_tmpbuf/2] != aq_max_tmpbuf/2){
    printf("array queue push element value is right ? test fail\n");
    test_err++;
    printf("now aq_buf[aq_max_tmpbuf/2]=%d\n",aq_test.aq_buf[aq_max_tmpbuf/2]);
  }
  if(aq_test.aq_buf[aq_max_tmpbuf/2-10] != aq_max_tmpbuf/2-10){
    printf("array queue push element value is right ? test fail\n");
    test_err++;
    printf("now aq_buf[aq_max_tmpbuf/2-10]=%d\n",aq_test.aq_buf[aq_max_tmpbuf/2-10]);
  }
  if(aq_test.aq_buf[aq_max_tmpbuf+aq_max_tmpbuf/2] != aq_max_tmpbuf/2){
    printf("array queue push element value is right ? test fail\n");
    test_err++;
    printf("now aq_buf[aq_max_tmpbuf+aq_max_tmpbuf/2]=%d\n",aq_test.aq_buf[aq_max_tmpbuf/2]);
  }
  //
  //弹空队列测试--测试情况是：pop 出时到达缓冲区极限，buf=4096
  memset(&aq_test.aq_tmpbuf_pop,0,aq_max_tmpbuf);//初始化弹出缓冲区
  for(tmp2 = 0;tmp2 < aq_max_buf/aq_max_tmpbuf;tmp2++){
    if(aq_popN(&aq_test) != aq_max_tmpbuf){
      printf("array queue pop element till empty test fail\n");
      test_err++;
    }
  }
  //对pop 出的数据进行抽样检查...如果pop 出正常, 那么pop 入的应该正常
  if(aq_test.aq_tmpbuf_pop[0] != 0){
    printf("array queue pop element value is right ? test fail\n");
    test_err++;
    printf("now aq_tmpbuf_pop[0]=%d\n",aq_test.aq_tmpbuf_pop[0]);
  }
  if(aq_test.aq_tmpbuf_pop[aq_max_tmpbuf-1] != aq_max_tmpbuf-1){
    printf("array queue pop element value is right ? test fail\n");
    test_err++;
    printf("now aq_tmpbuf_pop[aq_max_tmpbuf-1]=%d\n",aq_test.aq_tmpbuf_pop[aq_max_tmpbuf-1]);
  }
  if(aq_test.aq_tmpbuf_pop[aq_max_tmpbuf/2] != aq_max_tmpbuf/2){
    printf("array queue pop element value is right ? test fail\n");
    test_err++;
    printf("now aq_tmpbuf_pop[aq_max_tmpbuf/2]=%d\n",aq_test.aq_tmpbuf_pop[aq_max_tmpbuf/2]);
  }
  if(aq_test.aq_tmpbuf_pop[aq_max_tmpbuf/2-10] != aq_max_tmpbuf/2-10){
    printf("array queue pop element value is right ? test fail\n");
    test_err++;
    printf("now aq_tmpbuf_pop[aq_max_tmpbuf/2-10]=%d\n",aq_test.aq_tmpbuf_pop[aq_max_tmpbuf/2-10]);
  }
  //判断是否为空测试
  if(aq_get_size(&aq_test) != 0){
    printf("pop empty array queue test fail\n");
    test_err++;
  }
  //
  memset(&aq_test.aq_buf,0,40960);//还原queue buf
  printf("now front is %d\n",aq_test.aq_cur_pos_front);
  printf("now rear is %d\n",aq_test.aq_cur_pos_rear);
  //
  //常态push 入与pop 出测试
  for(tmp2 = 0;tmp2 < 10;tmp2++){//执行压满
    if(aq_pushN(&aq_test,20) != aq_test.aq_sig_push_ok){//200个元素-全部是0-19的数字
      printf("array queue push element=200 test fail\n");
      test_err++;
    }
  }
  //获取当前容量测试
  if(aq_get_size(&aq_test) != 200){
    printf("array queue get cur_size=200 test fail\n");
    test_err++;
  }
  //常态弹出队列测试--测试情况是：pop 出时到达缓冲区时未到达极限
  memset(&aq_test.aq_tmpbuf_pop,0,aq_max_tmpbuf);//初始化弹出缓冲区
  if(aq_popN(&aq_test) != 200){
    printf("array queue pop element=200 test fail\n");
    test_err++;
  }
  //抽样检查数据
  if(aq_test.aq_tmpbuf_pop[0] != 0){
    printf("array queue pop element value is right ? test fail\n");
    test_err++;
    printf("now aq_tmpbuf_pop[0]=%d\n",aq_test.aq_tmpbuf_pop[0]);
  }
  //
  if(aq_test.aq_tmpbuf_pop[10] != 10){
    printf("array queue pop element value is right ? test fail\n");
    test_err++;
    printf("now aq_tmpbuf_pop[10]=%d\n",aq_test.aq_tmpbuf_pop[10]);
  }
  //
  if(aq_test.aq_tmpbuf_pop[199] != 19){
    printf("array queue pop element value is right ? test fail\n");
    test_err++;
    printf("now aq_tmpbuf_pop[199]=%d\n",aq_test.aq_tmpbuf_pop[199]);
  }

  if(test_err == 0)
    printf("********************array queue test ok************************\n");
  else
    printf("********************array queue test wrong but finish************************\n");
  printf("important error notes print now: queue = %d\n", aq_test.aq_sig_msg_err);
  return;
}
