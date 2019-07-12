#include "v2_array.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

//构造函数
template<typename T> v2_array<T>::v2_array(int max_volume_x, int max_volume_y){
  //初始化private 变量
  this->max_volume_x = max_volume_x;
  this->max_volume_y = max_volume_y;
  this->stat_find = false;
  this->cur_pos = 0;

  *this->_pool_y = malloc(sizeof(void*) * this->max_volume_y);//创建数据池

  int tmp = 0;
  for(;tmp < max_volume_y;tmp++){
    this->_pool_y[tmp] = malloc(sizeof(T) * this->max_volume_x);
    memset(this->_pool_y[tmp], NULL, sizeof(T) * this->max_volume_x);
  }

  this->rwlock_v2 = PTHREAD_RWLOCK_INITIALIZER;//静态初始化-默认读写锁

  //初始化public 变量
  this->count_unused = 0;
  this->count_used = 0;

}

//释构函数
template<typename T> v2_array<T>::~v2_array(){
  //释放数据池
  int tmp = 0;
  for(;tmp < this->max_volume_y;tmp++)
    free(this->_pool_y[tmp]);
  free(*this->_pool_y);
  
}

