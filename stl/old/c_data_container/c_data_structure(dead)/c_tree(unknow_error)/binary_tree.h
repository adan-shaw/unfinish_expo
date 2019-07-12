/*
 * binary_tree.h
 *
 *  Created on: Nov 2, 2017
 *      Author: adan
 */

#include <stdio.h>
#include <malloc.h>

//二叉树元素
struct TNode{
  char data;
  struct TNode *lchild;
  struct TNode *rchild;
};

//元素结构体别名
typedef struct TNode Node;

//初始化二叉树(传入一个树根root--随便定义个指针即可: Node *root;)
void init(Node **node);

//构建二叉树(向二叉树压入单个char, 只是空间存储, 并不会排序)
void construct(char data, Node **node);

//前序遍历(先打印root, 后打印左边, 最后打印右边--先父后子)
int pre_order_traverse(Node *tree_node);

//后续遍历(先打印左边, 后打印右边, 最后打印root--先子后父)
int post_order_traverse(Node *tree_node);

//中序遍历(从小到大-中序遍历)
int in_order_traverse(Node *tree_node);

//子页个数(就是最底下的元素)
int leaf_num(Node *tree_node, int *count);

//求二叉树高度
int tree_height(Node *tree_node);

//普通二叉树测试
int binary_tree_test();

