/*
 * double_bubble_sort.h
 *
 *  Created on: Nov 4, 2017
 *      Author: adan
 */


#include "sort_test_public.h"
//双向冒泡: 假设从小到大排列(这个不影响实际效率)...
//升序: 第一次选出Max 到右边,
//降序: 第二次选出min 到左边..
//一次max, 一次min, 一直是这样循环, 直至一次都没有交换, 就quit

//这样做的好处就是, 将大部分中间值留在中间, 方便相邻的数冒泡交换, 彻底改进了冒泡算法...
//最大限度降低风险, 做到比较均匀的算法效率...冒泡max = o^2 的算法复杂度


//局限说明: 为了减少交换算法的值域限制问题, 尽量不用加法交换方案

//传入一个int 数组, 对数组进行双向冒泡排序, 另外必须知道数组元素的个数
//排序过程中不对数据正确性校验, 仅做排序
void double_bubble_sort(int* array, int count);

//双向冒泡测试
void double_bubble_sort_test(void);
