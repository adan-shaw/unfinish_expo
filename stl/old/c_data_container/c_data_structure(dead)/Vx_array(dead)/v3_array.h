#include <stdbool.h>
#include <pthread.h>

//定义宏
#define _unused_sig 0
//#define _element_type int //[int, float, double, unsigned int, long, longlong]

//三维数组类
template<typename T> class v3_array{//定义模板类型
private:
  pthread_rwlock_t rwlock_v3;//私有rw 锁
  bool stat_find;//查找状态
  int cur_pos;//空闲元素游标
  T* _pool_z;//数据池-不能直接访问--最高维的单维数组
  int max_volume_x;//数组容器当前容量
  int max_volume_y;
  int max_volume_z;

public:
  int count_unused;//数组中空闲的数量统计
  int count_used;//数组中已经使用的数量统计
  
  v3_array(int max_volumex_x, int max_volume_y, int max_volume_z);//构造函数
  ~v3_array();//释构函数

};
