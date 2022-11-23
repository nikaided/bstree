#ifndef RBTREE_H
#define RBTREE_H

#include <stdio.h>
#include <stdlib.h>

// マクロ定義
#define BLACK 	(0)
#define RED		(1)
#define RBTNIL (&sentinel)

// 型定義
typedef struct rbtree
{
    struct rbnode * root;
    int size;
} RBTree;

typedef struct rbnode
{
    int key;
    int count;
    char color;
    struct rbnode * parent;
    struct rbnode * left;
    struct rbnode * right;
} RBNode;

// プロトタイプ宣言
extern RBNode sentinel;
RBTree * create_rbtree(void);
void delete_rbtree(RBTree * );
void print_order(RBNode * );
RBNode * search(RBTree * , int);
RBNode * get_next(RBNode * );
RBNode * get_prev(RBNode * );
RBNode * insert(RBTree * , int);
RBNode * create_node(int);
void delete(RBTree * , RBNode * );

#endif
