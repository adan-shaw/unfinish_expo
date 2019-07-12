/*
 * test_main.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: adan
 */

#include "_array_stack.h"
#include "_array_queue.h"


//测试主函数
int main(void){
  _array_stack* pas = new _array_stack();
  delete pas;

  _array_queue* paq = new _array_queue();
  delete paq;

  return 0;
}
