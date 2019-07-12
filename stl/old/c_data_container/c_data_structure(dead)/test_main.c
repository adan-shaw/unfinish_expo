/*
 * test_main.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: adan
 */


#include "./c_linklist_str/linklist.h"

//#include "./c_tree/binary_tree.h"
//#include "./c_tree/rbtree_test.h"

#include "./c_sort/double_bubble_sort.h"
#include "./c_sort/quick_sort.h"
//实用性都不大, 用之前需要自己做深度改装...
//适合: 改造型开发(代码不一定稳定, 看自己能力)

//测试主函数
int main(void){
  //旧的c 树结构测试...调用接口
  //binary_tree_test();
  //rbtree_test_int_int();
  //rbtree_test_int_void();

  //旧的链表测试(字符串索引优化版)
  printf("******************************\n\n\n");
  linklist_test();

  //旧的排序算法测试
  printf("******************************\n\n\n");
  double_bubble_sort_test();//双向冒泡测试入口
  printf("******************************\n\n\n");
  quick_sort_test();//快速排序测试入口
  return 0;
}
