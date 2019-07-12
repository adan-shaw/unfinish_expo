//普通二叉树 -- 有前, 中, 后序遍历

#include "binary_tree.h"

//初始化二叉树
void init(Node **node){
  *node = (Node *)malloc(sizeof(Node));
  (*node)->lchild = (*node)->rchild = NULL;
  (*node)->data = 0;
}

//构建二叉树
void construct(char data, Node **node){
  Node *temp_node = *node;
  while(temp_node){
    if(!temp_node->data){
      temp_node->data = data;
      break;
    }
    else if(data <= temp_node->data){
      if(!temp_node->lchild){
        init(&temp_node->lchild);
        temp_node->lchild->data = data;
        break;
      }
	  else{
        temp_node = temp_node->lchild;
        continue;
      }
    }
    else if(data > temp_node->data){
      if(!temp_node->rchild){
        init(&temp_node->rchild);
        temp_node->rchild->data = data;
        break;
      }
	  else{
		temp_node = temp_node->rchild;
		continue;
	  }
	}
  }
  return;
}

//前序遍历--(先打印root, 后打印左边, 最后打印右边--先父后子)
int pre_order_traverse(Node *tree_node){
  if(tree_node){
	if(printf("%c  ", tree_node->data))
	  if(pre_order_traverse(tree_node->lchild))
		if(pre_order_traverse(tree_node->rchild))
		  return 1;
	return 0;
  }
  else
	return 1;
}

//后续遍历-(先打印左边, 后打印右边, 最后打印root--先子后父)
int post_order_traverse(Node *tree_node){
  if(tree_node){
	if(post_order_traverse(tree_node->lchild))
	  if(post_order_traverse(tree_node->rchild))
		if(printf("%c  ", tree_node->data))
		  return 1;
    return 0;
  }
  else
    return 1;
}

//中序遍历(从小到大-中序遍历)
int in_order_traverse(Node *tree_node){
  if(tree_node){
    if(in_order_traverse(tree_node->lchild))
	  if(printf("%c  ", tree_node->data))
		if(in_order_traverse(tree_node->rchild))
		  return 1;
    return 0;
  }
  else
    return 1;
}

//子页个数(就是最底下的元素)
int leaf_num(Node *tree_node, int *count){
  if(tree_node){
	if(!tree_node->lchild && !tree_node->rchild)
	  (*count)++;
	if(leaf_num(tree_node->lchild, count))
	  if(leaf_num(tree_node->rchild, count))
		return 1;
	return 0;
  }
  else
    return 1;
}

//求二叉树高度
int tree_height(Node *tree_node){
  int h1, h2;
  if(!tree_node)
    return -1;
  else{
    h1 = tree_height(tree_node->lchild);
    h2 = tree_height(tree_node->rchild);
    return h1>h2? (h1+1): (h2+1);
  }
}

//普通二叉树测试
int binary_tree_test(){
  int i, count = 0;
  Node *root;
  char data[11] = {'e','f','h','g','a','c','b','d','x','z','u'};
  init(&root);
  for(i = 0; i < 11; i++)
    construct(data[i], &root);
  printf("***********************\n");
  printf("普通二叉树测试函数开始\n");
  printf("***********************\n");
  //前序遍历
  printf("pre_order_traverse..................\n");
  pre_order_traverse(root);

  //后续遍历
  printf("\npost_order_traverse..................\n");
  post_order_traverse(root);

  //中序遍历
  printf("\nin_order_traverse..................\n");
  in_order_traverse(root);

  printf("\ncounting leaf_number\n");
  leaf_num(root, &count);
  printf("leaf number is %d\n", count);

  printf("the height of tree is %d\n", tree_height(root));
  return 0;
}
