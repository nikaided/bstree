CFLAGS=-std=c11 -g -static

rbtree: rbtree_test.c rbtree.c rbtree.h
	clang -o rbtree rbtree_test.c rbtree.c