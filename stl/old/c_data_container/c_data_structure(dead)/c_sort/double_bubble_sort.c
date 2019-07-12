/*
 * double_bubble_sort.c
 *
 *  Created on: Nov 4, 2017
 *      Author: adan
 */

#include "double_bubble_sort.h"
//传入一个int 数组, 对数组进行双向冒泡排序, 另外必须知道数组元素的个数
//排序过程中不对数据正确性校验, 仅做排序
void double_bubble_sort(int* array, int count){
  int for_out = count - 1;//n 个元素要做n-1 次比较
  int for_in = for_out;//每次比较完毕, 都产生一个max or min, 内循环可减少一次比较
  int for_out_tmp = 0;
  int for_in_tmp = 0;

  int sig = 0;//风向标-max,min 切换
  int for_in_pos_max = 0;//每次产生确定值, 这里都可以减少一次比较
  int for_in_pos_min = count - 1;
  int for_in_pos = for_in_pos_max;
  int quit_tmp = 0;//没有交换就break

  int tmp;
  for(;for_out_tmp < for_out;for_out_tmp++){//外循环-元素个数
	for(;for_in_tmp < for_in;for_in_tmp++){//内循环,比较次数
      if(sig == 0){//向右
    	//printf("%d-%d\n",array[for_in_pos],array[for_in_pos+1]);//for test
    	if(array[for_in_pos] > array[for_in_pos+1]){
          tmp = array[for_in_pos];//执行交换
          array[for_in_pos] = array[for_in_pos+1];
          array[for_in_pos+1] = tmp;
          quit_tmp++;//只要交换过都不让退出
    	}
    	for_in_pos++;
      }
      else{//向左
    	//printf("%d-%d\n",array[for_in_pos-1],array[for_in_pos]);//for test
	    if(array[for_in_pos] < array[for_in_pos-1]){
	      tmp = array[for_in_pos];
	      array[for_in_pos] = array[for_in_pos-1];
	      array[for_in_pos-1] = tmp;
	      quit_tmp++;
		}
	    for_in_pos--;
      }
	}
	//printf("\n");//for test
	if(sig == 0){
	  sig = 1;//更改风向
	  for_in_pos_min--;//比较起始pos 修改
	  for_in_pos = for_in_pos_min;//继承起始位置
	}
	else{
	  sig = 0;
	  for_in_pos_max++;
	  for_in_pos = for_in_pos_max;
	}
	for_in_tmp = 0;//还原这个临时变量
	for_in--;//比较次数减1

    //没有任何交换, 说明排序完毕
    if(quit_tmp == 0)
      break;
    else
      quit_tmp = 0;
  }
  return;
}




//双向冒泡测试
void double_bubble_sort_test(void){
  printf("double_bubble_sort_test start\n");
  printf("double_bubble_sort_test start\n");
  printf("double_bubble_sort_test start\n");
  int xx[20] = {9,3,5,7,8,1,2,6,4,11,24,36,45,12,12,12,16,48,57,69};
  //测试数量级
  int *x100 = make_random_array(100);
  int *x1k = make_random_array(1000);
  int *x1w = make_random_array(10000);//1秒内--看来1w 是临界点
  int *x10w = make_random_array(100000);//几十秒-26s 左右
  //int *x100w = make_random_array(1000000);//噩梦-注释掉了
  //耗时统计
  char start[64];
  char end[64];

  //双向冒泡测试
  get_xtime(&start);
  double_bubble_sort(xx,20);
  get_xtime(&end);
  printf("exec start and finish time: \n%s%s", start, end);
  parray(xx,20);//可以不打印
  if(check_sort(xx,20) == 0)
	printf("double_bubble_sort(xx) success !\n");
  else
	printf("double_bubble_sort(xx) fail !\n");

  printf("\n\n");
  get_xtime(&start);
  double_bubble_sort(x100,100);
  get_xtime(&end);
  printf("exec start and finish time: \n%s%s", start, end);
  //parray(x100,100);//可以不打印
  if(check_sort(x100,100) == 0)
	printf("double_bubble_sort(x100) success !\n");
  else
	printf("double_bubble_sort(x100) fail !\n");

  printf("\n\n");
  get_xtime(&start);
  double_bubble_sort(x1k,1000);
  get_xtime(&end);
  printf("exec start and finish time: \n%s%s", start, end);
  //parray(x100,100);//可以不打印
  if(check_sort(x1k,1000) == 0)
    printf("double_bubble_sort(x1k) success !\n");
  else
  	printf("double_bubble_sort(x1k) fail !\n");


 printf("\n\n");
  get_xtime(&start);
  double_bubble_sort(x1w,10000);
  get_xtime(&end);
  printf("exec start and finish time: \n%s%s", start, end);
  //parray(x100,100);//可以不打印
  if(check_sort(x1w,10000) == 0)
	printf("double_bubble_sort(x1w) success !\n");
  else
	printf("double_bubble_sort(x1w) fail !\n");

  printf("\n\n");
  get_xtime(&start);
  double_bubble_sort(x10w,100000);
  get_xtime(&end);
  printf("exec start and finish time: \n%s%s", start, end);
  //parray(x100,100);//可以不打印
  if(check_sort(x10w,100000) == 0)
    printf("double_bubble_sort(x10w) success !\n");
  else
    printf("double_bubble_sort(x10w) fail !\n");

  /*
  printf("\n\n");
  get_xtime(&start);
  double_bubble_sort(x100w,1000000);
  get_xtime(&end);
  printf("exec start and finish time: \n%s%s", start, end);
  //parray(x100,100);//可以不打印
  if(check_sort(x100w,1000000) == 0)
    printf("double_bubble_sort(x100w) success !\n");
  else
    printf("double_bubble_sort(x100w) fail !\n");
  */

  free(x100);
  free(x1k);
  free(x1w);
  free(x10w);
  //free(x100w);
  return;
}

