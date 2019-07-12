/*
 * sort_test_public.c
 *
 *  Created on: Dec 5, 2017
 *      Author: adan
 */


#include "sort_test_public.h"//随机数产生器


//输入要产生的随机数个数count, 返回一个malloc 指针
int* make_random_array(int count){
  int* buf = malloc(count*sizeof(int));
  int tmp = 0;
  for(;tmp < count;tmp++)
	buf[tmp] = get_random_sort();
  return buf;
}

//获取随机数-无字典
int get_random_sort(void){
  int _clock = clock();//获取CPU 时钟	-- 取4位
  int _time = time(NULL);//获取时间		-- 取4位
  int _clock4 = _clock % 10000;
  int _time4 = _time % 10000;
  int result = _clock4*_time4%1000000;
  srand(result);//设置随机数种子
  //if(rand() + result - 2056 < 0)
  //printf("%d",rand() + result - 2056);//for test
  return rand()%1000000 + result - 7294;
}


//获取一个当前时间的字符串到实参中(实参应为64 bit 的char)
void get_xtime(char* buf_64){
  time_t t = time(NULL);
  struct tm *_tm;
  _tm = localtime(&t);
  //asctime(_tm);//转为localtime string
  sprintf(buf_64,"%s",asctime(_tm));
}

//打印数组
void parray(int* array, int count){
  int tmp = 0;
  int x = 10;
  for(;tmp < count;tmp++){
    printf("%d-",array[tmp]);
    if(x == 0){
      printf("\n");
      x = 10;
    }
    else
      x++;
  }
  return;
}

//检查排序的正确性(统一都是从小到大) -- 排序正确返回0, 失败返回-1
int check_sort(int* array, int count){
  int tmp = 0;
  int xcount = 1;
  for(;tmp < count-1;tmp++){
    if(array[tmp] > array[tmp+1]){
      printf("%d--%d,%d\n",tmp,array[tmp],array[tmp+1]);
      return -1;
    }
    else
      xcount++;
  }

  if(xcount == count)
    return 0;//一次都没有错, 返回成功
  else
	return -1;
}


