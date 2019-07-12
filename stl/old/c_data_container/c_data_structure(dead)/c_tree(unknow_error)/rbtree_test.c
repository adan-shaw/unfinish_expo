/*
 * rbtree_test.c
 *
 *  Created on: Oct 2, 2017
 *      Author: adan
 */

#include "rbtree_test.h"

//*************************************************************************
//*************************************************************************
//红黑树查找--成功返回<struct rbt_data>, 失败返回NULL
struct rbt_data* rbt_search(struct rb_root *root, rbtree_type_key key){
  struct rb_node *node = root->rb_node;

  while(node){
	//取出当前树节点的数据部分
    struct rbt_data *data = container_of(node, struct rbt_data, data_node);
    //二分法
    if(key < data->key)
      node = node->rb_left;//少于key < data->key, 向左树继续搜索
    else if (key > data->key)
      node = node->rb_right;
    else
      return data;//key = data->key, 则返回树节点的数据部分
  }
  return NULL;
}


//红黑树插入(不能插入已经存在的数据) 成功返回0, 失败返回-1
int rbt_insert(struct rb_root *root, struct rbt_data *data){
  struct rb_node **tmp = &(root->rb_node);
  struct rb_node *parent = NULL;

  while (*tmp){
    struct rbt_data *cur = container_of(*tmp, struct rbt_data, data_node);

    parent = *tmp;
    if(data->key < cur->key)
      tmp = &((*tmp)->rb_left);
    else if(data->key > cur->key)
      tmp = &((*tmp)->rb_right);
    else
      return -1;
  }
  rb_link_node(&(data->data_node), parent, tmp);
  rb_insert_color(&(data->data_node), root);

  return 0;
}


//红黑树删除(成功删除返回0, 失败返回-1)
int rbt_delete(struct rb_root *root, rbtree_type_key key){
  //(由于没有保险机制, 必须rbt_search() 确认元素存在才能执行删除...这个是设计缺陷问题)
  struct rbt_data *data = rbt_search(root, key);
  if(!data)
    return -1;
  rb_erase(&(data->data_node), root);
  free(data);
  return 0;
}
//*************************************************************************
//*************************************************************************


//打印整棵树-int_int(从小到大-中序遍历)
void print_rbtree_int_int(struct rb_root *root){
  struct rb_node *node;

  for(node = rb_first(root); node; node = rb_next(node)){
    printf("%d-", rb_entry(node, struct rbt_data, data_node)->key);
    printf("%d ", rb_entry(node, struct rbt_data, data_node)->val);
  }

  printf("\n");
}

//打印整棵树-int_void(从小到大-中序遍历)
void print_rbtree_int_void(struct rb_root *root){
  struct rb_node *node;

  for(node = rb_first(root); node; node = rb_next(node)){
    printf("%d-", rb_entry(node, struct rbt_data, data_node)->key);
    printf("%s ", rb_entry(node, struct rbt_data, data_node)->val);
  }

  printf("\n");
}

//测试主函数-int_int  --  //gcc rbtree_test.c rbtree.c -o test
int rbtree_test_int_int(void){
  printf("***********************\n");
  printf("内核红黑树测试函数开始, int-int\n");
  printf("***********************\n");

  struct rb_root root = RB_ROOT;
  struct rbt_data *data;
  int i;

  printf("struct rbt_data len:%ld, please enter 5 integers:\n", sizeof(struct rbt_data));
  for(i=0; i<5; i++){
    data = malloc(sizeof(struct rbt_data));//分配节点数据内存struct rbt_data
    if(!data){
      perror("fail to malloc.");
    }

    //键-值 赋值
    //scanf("%d", &data->key);
    data->key = i*3 + 1;
    data->val = i*2 + 2;

    int ret = rbt_insert(&root, data);//插入节点
    if (ret < 0){
      //节点已经存在
      fprintf(stderr, "the %d already exists.\n", data->key);
      i--;
      free(data);//已经存在的节点..释放节点内存or 你可以做其它操作, 如累计
    }
  }

  printf("\nprint whole rbtree:\n");
  print_rbtree_int_int(&root);//打印整棵树

  int key;
  printf("\nplease enter the key to delete:\n");
  scanf("%d", &key);
  if(rbt_delete(&root, key) != 0)//删除节点
    printf("rbtree error: not fount %d.\n", key);

  printf("\ndelete after, print whole rbtree:\n");
  print_rbtree_int_int(&root);

  return 0;
}

//测试主函数-int_void
#include <string.h>
int rbtree_test_int_void(void){
  printf("***********************\n");
  printf("内核红黑树测试函数开始, int-void\n");
  printf("***********************\n");

  struct rb_root root = RB_ROOT;
  struct rbt_data *data;
  int i;

  printf("struct rbt_data len:%ld, please enter 5 integers:\n", sizeof(struct rbt_data));
  for(i=0; i<5; i++){
    data = malloc(sizeof(struct rbt_data));//分配节点数据内存struct rbt_data
    if(!data){
      perror("fail to malloc.");
    }

    //键-值 赋值
    //scanf("%d", &data->key);
    data->key = i*3 + 1;
    data->val = malloc(16);
    memset(data->val,'\0',16);
    memcpy(data->val,"love you",8);

    int ret = rbt_insert(&root, data);//插入节点
    if (ret < 0){
      //节点已经存在
      fprintf(stderr, "the %d already exists.\n", data->key);
      i--;
      free(data);//已经存在的节点..释放节点内存or 你可以做其它操作, 如累计
    }
  }

  printf("\nprint whole rbtree:\n");
  print_rbtree_int_void(&root);//打印整棵树

  int key;
  printf("\nplease enter the key to delete:\n");
  scanf("%d", &key);
  if(rbt_delete(&root, key) != 0)//删除节点
    printf("rbtree error: not fount %d.\n", key);

  printf("\ndelete after, print whole rbtree:\n");
  print_rbtree_int_void(&root);

  return 0;
}
