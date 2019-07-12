#include "array_queue.h"
#include "array_stack.h"

//有很多缺陷不能使用
//1.int 没有限制值域
//2.互斥锁(这个也没影响啦)
//3.paq->aq_sig_full = -4000000000 标记值设计不好, 但是勉强可以用
//4.

int main(void){
  aq_test();
  as_test();
  return 0;
}
