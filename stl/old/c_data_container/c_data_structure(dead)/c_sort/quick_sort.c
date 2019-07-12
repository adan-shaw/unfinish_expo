/*
 * quick_sort.c
 *
 *  Created on: Nov 4, 2017
 *      Author: adan
 */

#include "quick_sort.h"

//快速排序的实现
void quick_sort(int array[], int maxlen, int begin, int end){
  int i, j;
  int xtmp;
  if(begin < end){
    i = begin + 1; // 将array[begin]作为基准数，因此从array[begin+1]开始与基准数比较！
    j = end;       // array[end]是数组的最后一位

    while(i < j){
      if(array[i] > array[begin]){ // 如果比较的数组元素大于基准数，则交换位置
    	  xtmp = array[i];
    	  array[i] = array[j];
    	  array[j] = xtmp;
        j--;
      }
      else
        i++; // 将数组向后移一位，继续与基准数比较
    }

	/* 跳出while循环后，i = j。
	 * 此时数组被分割成两个部分  -->  array[begin+1] ~ array[i-1] < array[begin]
	 *                       -->  array[i+1] ~ array[end] > array[begin]
	 * 这个时候将数组array分成两个部分，再将array[i]与array[begin]进行比较，决定array[i]的位置。
	 * 最后将array[i]与array[begin]交换，进行两个分割部分的排序！以此类推，直到最后i = j不满足条件就退出！
	 */

    if(array[i] >= array[begin]) // 这里必须要取等“>=”，否则数组元素由相同的值时，会出现错误！
      i--;

    xtmp = array[begin];
    array[begin] = array[i];
    array[i] = xtmp;

    quick_sort(array, maxlen, begin, i);
    quick_sort(array, maxlen, j, end);
  }
}


//快速排序测试
void quick_sort_test(void){
  printf("quick_sort_test start\n");
  printf("quick_sort_test start\n");
  printf("quick_sort_test start\n");
  int xx[20] = {9,3,5,7,8,1,2,6,4,11,24,36,45,12,12,12,16,48,57,69};
  //测试数量级
  int *x100 = make_random_array(100);
  int *x1k = make_random_array(1000);
  int *x1w = make_random_array(10000);
  int *x10w = make_random_array(100000);
  int *x100w = make_random_array(1000000);//仅用1秒
  //耗时统计
  char start[64];
  char end[64];

  //开始测试
  get_xtime(&start);
  quick_sort(xx,20,0,19);
  get_xtime(&end);
  printf("exec start and finish time: \n%s%s", start, end);
  parray(xx,20);//可以不打印
  if(check_sort(xx,20) == 0)
	printf("quick_sort(xx) success !\n");
  else
	printf("quick_sort(xx) fail !\n");

  printf("\n\n");
  get_xtime(&start);
  quick_sort(x100,100,0,99);
  get_xtime(&end);
  printf("exec start and finish time: \n%s%s", start, end);
  //parray(x100,100);//可以不打印
  if(check_sort(x100,100) == 0)
	printf("quick_sort(x100) success !\n");
  else
	printf("quick_sort(x100) fail !\n");

  printf("\n\n");
  get_xtime(&start);
  quick_sort(x1k,1000,0,999);
  get_xtime(&end);
  printf("exec start and finish time: \n%s%s", start, end);
  //parray(x100,100);//可以不打印
  if(check_sort(x1k,1000) == 0)
    printf("quick_sort(x1k) success !\n");
  else
  	printf("quick_sort(x1k) fail !\n");


 printf("\n\n");
  get_xtime(&start);
  quick_sort(x1w,10000,0,9999);
  get_xtime(&end);
  printf("exec start and finish time: \n%s%s", start, end);
  //parray(x100,100);//可以不打印
  if(check_sort(x1w,10000) == 0)
	printf("quick_sort(x1w) success !\n");
  else
	printf("quick_sort(x1w) fail !\n");

  printf("\n\n");
  get_xtime(&start);
  quick_sort(x10w,100000,0,99999);
  get_xtime(&end);
  printf("exec start and finish time: \n%s%s", start, end);
  //parray(x100,100);//可以不打印
  if(check_sort(x10w,100000) == 0)
    printf("quick_sort(x10w) success !\n");
  else
    printf("quick_sort(x10w) fail !\n");


  printf("\n\n");
  get_xtime(&start);
  quick_sort(x100w,1000000,0,999999);
  get_xtime(&end);
  printf("exec start and finish time: \n%s%s", start, end);
  //parray(x100,100);//可以不打印
  if(check_sort(x100w,1000000) == 0)
    printf("quick_sort(x100w) success !\n");
  else
    printf("quick_sort(x100w) fail !\n");


  free(x100);
  free(x1k);
  free(x1w);
  free(x10w);
  free(x100w);
  return;
}
