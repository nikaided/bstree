CFLAGS=-std=c11 -g -static

rbtree: main.c rbtree.c rbtree.h
	clang main.c rbtree.c