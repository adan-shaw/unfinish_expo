#include "v1_array.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

//构造函数
template<typename T> v1_array<T>::v1_array(int max_volume_x){
  //初始化private 变量
  this->max_volume_x = max_volume_x;
  this->stat_find = false;
  this->cur_pos = 0;
  
  this->_pool_x = (T*)malloc(sizeof(T) * this->max_volume_x);//创建数据池
  memset(this->_pool_x, 0, sizeof(T) * this->max_volume_x);

  this->rwlock_v1 = PTHREAD_RWLOCK_INITIALIZER;//静态初始化-默认读写锁

  //初始化public 变量
  this->count_unused = 0;
  this->count_used = 0;

}

//释构函数
template<typename T> v1_array<T>::~v1_array(){
  free(this->_pool_x);//释放数据池
}
