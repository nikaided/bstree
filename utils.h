#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

// macro definition
#define BLACK (0)
#define RED (1)
#define RBTNIL (&sentinel)

#define COMPARE_ERR INT_MIN
#define INC_REF 1
#define KEEP_REF 0
#define DEC_REF -1

// whether tree holds duplicated key or not
// if so, node count will increase.
#define NO_DUP 0
#define DUP 1

typedef enum
{
    KEY_LONG,
    KEY_DOUBLE,
    KEY_OBJECT
} KeyType;

typedef union key_tag
{
    long lval;
    double dval;
    PyObject *obj;
} Key;

typedef struct
{
    KeyType type;
    Key value;
} TypedKey;

typedef struct objnode
{
    PyObject *obj;
    struct objnode *next;
} ObjNode;

typedef struct rbnode
{
    // the key of the object to compare
    TypedKey *key;
    // linked list of python objects with the same key
    ObjNode *obj_list;
    // the number of elements of obj_list
    unsigned long count;
    char color;
    // the number of nodes in the subtree rooted at this node
    unsigned long size;
    struct rbnode *parent;
    struct rbnode *left;
    struct rbnode *right;
} RBNode;

typedef int (*CompareOperator)(TypedKey *, TypedKey *);

typedef struct
{
    PyObject_HEAD RBNode *root;
    unsigned long size;
    char is_dup;
    CompareOperator ope;
    PyObject *keyfunc;
    PyObject *captured;
} BSTreeObject;

// functions of create and delete rbnodes/objnodes
RBNode *_create_rbnode(TypedKey *);
void _delete_rbnode(RBNode *);
void _delete_all_rbnodes(RBNode *);
ObjNode *_create_objnode(PyObject *);
int _add_objnode_to_rbnode(ObjNode *, RBNode *);
int _delete_obj_from_rbnode(RBNode *);

// functions of reading tree
RBNode *_search(TypedKey *, RBNode *, CompareOperator);
RBNode *_search_leaf(TypedKey *, RBNode *, CompareOperator);
PyObject *_list_in_order(RBNode *, PyObject *, int *, char);
int _add_counter(RBNode *, PyObject *);
RBNode *_get_min(RBNode *);
RBNode *_get_max(RBNode *);
RBNode *_get_next(RBNode *);
RBNode *_get_prev(RBNode *);
long _get_rank(TypedKey *, RBNode *, CompareOperator);
int _helper_smallest(RBNode *, unsigned long, PyObject **);
int _helper_largest(RBNode *, unsigned long, PyObject **);

// functions of writing tree
void _left_rotate(BSTreeObject *, RBNode *);
void _right_rotate(BSTreeObject *, RBNode *);
void _insert_fixup(BSTreeObject *, RBNode *);
void _delete_fixup(BSTreeObject *, RBNode *);
void _update_size(BSTreeObject *, RBNode *);
void _transplant(BSTreeObject *, RBNode *, RBNode *);

// functions of comparing pyobjects
TypedKey *get_key_from(PyObject *, PyObject *);
int _compare(TypedKey *, TypedKey *, CompareOperator);
int _lt(TypedKey *, TypedKey *);
int _lt_obj(PyObject *, PyObject *);
int get_long_from(PyObject *, long *);
int get_double_from(PyObject *, double *);

extern RBNode sentinel;

#endif // UTILS_H
