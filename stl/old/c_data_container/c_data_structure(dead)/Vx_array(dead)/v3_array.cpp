#include "v3_array.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

//构造函数
template<typename T> v3_array<T>::v3_array(int max_volumex_x, int max_volume_y, int max_volume_z){
  //初始化private 变量
  this->max_volume_x = max_volume_x;
  this->max_volume_y = max_volume_y;
  this->max_volume_z = max_volume_z;
  this->stat_find = false;
  this->cur_pos = 0;

  this->_pool_z = (T*)malloc(sizeof(T) * this->max_volume_z);//创建数据池
  memset(this->_pool_z, NULL, sizeof(T) * this->max_volume_z);

  int tmp = 0;
  for(;tmp < max_volume_z;tmp++){
    this->_pool_z[tmp] = (T*)malloc(sizeof(T) * this->max_volume_y);
    memset(this->_pool_z[tmp], NULL, sizeof(T) * this->max_volume_y);
    int tmp2 = 0;
    for(;tmp2 < max_volume_y;tmp2++){
      this->_pool_z[tmp][tmp2] = (T*)malloc(sizeof(T) * this->max_volume_x);
      memset(this->_pool_z[tmp][tmp2], NULL, sizeof(T) * this->max_volume_x);
    }
  }

  this->rwlock_v3 = PTHREAD_RWLOCK_INITIALIZER;//静态初始化-默认读写锁

  //初始化public 变量
  this->count_unused = 0;
  this->count_used = 0;

}

//释构函数
template<typename T> v3_array<T>::~v3_array(){
  free(this->_pool_z);//释放数据池
}

