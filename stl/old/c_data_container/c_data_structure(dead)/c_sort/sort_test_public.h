/*
 * sort_test_public.h
 *
 *  Created on: Dec 5, 2017
 *      Author: adan
 */

#include <stdio.h>
#include <time.h>

//输入要产生的随机数个数count, 返回一个malloc 指针
int* make_random_array(int count);

//获取随机数-无字典
int get_random_sort(void);

//获取一个当前时间的字符串到实参中(实参应为64 bit 的char)
void get_xtime(char* buf_64);

//打印数组
void parray(int* array, int count);

//检查排序的正确性(统一都是从小到大) -- 排序正确返回0, 失败返回-1
int check_sort(int* array, int count);
