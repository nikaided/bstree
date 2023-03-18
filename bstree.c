﻿#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#ifndef RBTREE_H
#define RBTREE_H

#include <stdio.h>
#include <stdlib.h>

// マクロ定義
#define BLACK 	(0)
#define RED		(1)
#define RBTNIL (&sentinel)

typedef struct
{
    PyObject_HEAD
    struct rbnode * root;
    unsigned size;
    PyObject * captured;
} BSTreeObject;

typedef struct rbnode
{
    long key;
    int count;
    char color;
    struct rbnode * parent;
    struct rbnode * left;
    struct rbnode * right;
} RBNode;

// プロトタイプ宣言
RBNode sentinel;
PyObject * bstree_insert(BSTreeObject * , PyObject *);
PyObject * bstree_search(BSTreeObject * , PyObject *);
PyObject * bstree_delete(BSTreeObject * , PyObject *);
PyObject * bstree_list(BSTreeObject * , PyObject *);
PyObject * Bstree_print(BSTreeObject * , PyObject *);
PyObject * Bstree_min(BSTreeObject * , PyObject *);
PyObject * Bstree_max(BSTreeObject * , PyObject *);


void _left_rotate(BSTreeObject * , RBNode * );
void _right_rotate(BSTreeObject * , RBNode * );
void _insert_fixup(BSTreeObject * , RBNode * );
void _delete_fixup(BSTreeObject * , RBNode * );
void _transplant(BSTreeObject * , RBNode * , RBNode * );

RBNode * _get_min(RBNode * );
RBNode * _get_max(RBNode * );
RBNode * get_next(RBNode * );
RBNode * get_prev(RBNode * );
RBNode * create_node(long);

#endif

// leafの変数：すべて同じひとつのnodeとして扱う
RBNode sentinel =
{
	.color = BLACK,
    .left = RBTNIL,
    .right = RBTNIL,
    .parent = NULL
};

// methodの定義
int
bstree_init(BSTreeObject * self, PyObject * args)
{
    self->root = RBTNIL;
    self->size = 0;
    return 0;
}

static PyObject * 
bstree_insert(BSTreeObject * self, PyObject * args)
{
    long key;
    if (!PyArg_ParseTuple(args, "l", &key))
    {
        return NULL;
    }
    // nodeを生成
    RBNode * nodep = create_node(key);
    self->size += 1;

    RBNode * yp = RBTNIL;
    RBNode * xp = self->root;
    while (xp!=RBTNIL)
    {
        yp = xp;
        if (nodep->key < xp->key)
            xp = xp->left;
        else if(nodep->key > xp->key)
            xp = xp->right;
        else
        {
            xp->count += 1;
            free(nodep);
            Py_RETURN_NONE;
            // return xp;
        }
    }
    nodep->parent = yp;
    if (yp==RBTNIL)
        self->root = nodep;
    else if (nodep->key < yp->key)
        yp->left = nodep;
    else
        yp->right = nodep;
    nodep->color = RED;
    _insert_fixup(self, nodep);
    Py_RETURN_NONE;
}

static PyObject * 
bstree_delete(BSTreeObject * self, PyObject * args)
{
    long key;
    RBNode * nodep;
    RBNode * _search(BSTreeObject *, int);

    if (!PyArg_ParseTuple(args, "l", &key))
        return NULL;

    if ((nodep = _search(self, key)) == RBTNIL)
        return NULL;
    
    self->size -= 1;

    RBNode * yp = nodep;
    RBNode * xp;
    char y_original_color = yp->color;
    
    if (nodep->count > 1)
    {
        nodep->count -= 1;
        Py_RETURN_NONE;
    }
    if (nodep->left == RBTNIL)
    {
        xp = nodep->right;
        _transplant(self, nodep, xp);
    }
    else if (nodep->right == RBTNIL)
    {
        xp = nodep->left;
        _transplant(self, nodep, xp);
    }
    else
    {
        yp = _get_min(nodep->right);
        y_original_color = yp->color;
        xp = yp->right;
        if (yp->parent == nodep)
            xp->parent = yp;
        else
        {
            _transplant(self, yp, yp->right);
            yp->right = nodep->right;
            yp->right->parent = yp;
        }
        _transplant(self, nodep, yp);
        yp->left = nodep->left;
        yp->left->parent = yp;
        yp->color = nodep->color;
        if (y_original_color == BLACK)
            _delete_fixup(self, xp);
    }
    free(nodep);
    Py_RETURN_NONE;
}

static PyObject * 
bstree_search(BSTreeObject * self, PyObject * args)
{
    long key;
    if (!PyArg_ParseTuple(args, "l", &key))
        return NULL;

    if (_search(self, key) == RBTNIL)
        return Py_False;
    else
        return Py_True;
}

// 昇順に値を並べたリストをリターンする
static PyObject *
bstree_list(BSTreeObject * self, PyObject * args)
{
    PyObject * _list_in_order(RBNode * , PyObject * ,int *);
    PyObject * list;
    int idx;
    idx = 0;
    list = PyList_New(self->size);
    RBNode * node = self->root;
    return _list_in_order(node, list, &idx);
}

static PyObject *
_list_in_order(RBNode * node, PyObject * list, int * pidx)
{
    if (node->left!=RBTNIL)
        list = _list_in_order(node->left, list, pidx);

    for (int i = 0; i < node->count; i++)
        PyList_SET_ITEM(list, *pidx+i, PyLong_FromLong(node->key));
    *pidx += node->count;

    if (node->right!=RBTNIL)
        list = _list_in_order(node->right, list, pidx);
    
    return list;
}


// 昇順に値を並べる
static PyObject *
bstree_print(BSTreeObject * self, PyObject * args)
{
    void _print_in_order(RBNode * );
    RBNode * node = self->root;
    _print_in_order(node);
    printf("\n");
    Py_RETURN_NONE;
}

static PyObject * 
bstree_min(BSTreeObject * self, PyObject * args)
{
    RBNode * _get_min(RBNode * );
    RBNode * nodep = _get_min(self->root);
    if (nodep==RBTNIL)
        return NULL;
    Py_BuildValue("l", nodep->key);     
}

static PyObject * 
bstree_max(BSTreeObject * self, PyObject * args)
{
    RBNode * _get_max(RBNode * );
    RBNode * nodep = _get_max(self->root);
    if (nodep==RBTNIL)
        return NULL;
    Py_BuildValue("l", nodep->key);     
}

// 
void _print_in_order(RBNode * node)
{
    if (node->left!=RBTNIL)
        _print_in_order(node->left);
    for (int i = 0; i < node->count; i++)
        printf("%ld, ", node->key);
    if (node->right!=RBTNIL)
        _print_in_order(node->right);
    return;
}

// get the node which key is k.
// If not exist, get RBTNIL.
RBNode * _search(BSTreeObject * self, int k)
{
    RBNode * zp = self->root;
    while (zp != RBTNIL && k != zp->key)
    {
        if (k < zp->key)
            zp = zp->left;
        else
            zp = zp->right;
    }
    return zp;
}

RBNode * create_node(long key)
{
    RBNode * nodep = malloc(sizeof(RBNode));
    if (nodep==NULL)
        return NULL;
    nodep->key = key;
    nodep->count = 1;
    nodep->parent = RBTNIL;
    nodep->left = RBTNIL;
    nodep->right = RBTNIL;
    return nodep;
}

// nodepをrootにしたときの木の最小値
RBNode * _get_min(RBNode * nodep)
{
    RBNode * zp = nodep;
    while (zp->left != RBTNIL)
        zp = zp->left;
    return zp;
}

// nodepをrootにしたときの木の最大値
RBNode * _get_max(RBNode * nodep)
{
    RBNode * zp = nodep;
    while (zp->right != RBTNIL)
        zp = zp->right;
    return zp;
}

// nodepの次に大きい値のノードを返す.
// 存在しないときはRBTNILを返す
// nodepはtreeに組み込まれている前提
RBNode * get_next(RBNode * nodep)
{
    RBNode * _get_min(RBNode * );

    if (nodep->right!=RBTNIL)
        return _get_min(nodep->right);

    RBNode * pp = nodep->parent;
    while (pp!=RBTNIL && nodep==pp->right)
    {
        nodep = pp;
        pp = nodep->parent;
    } 
    return pp;
}

// nodeはtreeに組み込まれている前提
RBNode * 
get_prev(RBNode * nodep)
{
    RBNode * _get_max(RBNode * );

    if (nodep->left!=RBTNIL)
        return _get_max(nodep->left);
    RBNode * pp = nodep->parent;
    while (pp!=RBTNIL && nodep==pp->left)
    {
        nodep = pp;
        pp = nodep->parent;
    } 
    return pp; 
}

static void
_left_rotate(BSTreeObject * self, RBNode * nodep)
{
    RBNode * yp = nodep->right;
    nodep->right = yp->left;
    if (yp->left != RBTNIL)
        yp->left->parent = nodep;
    yp->parent = nodep->parent;
    if (nodep->parent==RBTNIL)
        self->root = yp;
    else if (nodep==nodep->parent->left)
        nodep->parent->left = yp;
    else
        nodep->parent->right = yp;
    yp->left = nodep;
    nodep->parent = yp;
}

static void
_right_rotate(BSTreeObject * self, RBNode * nodep)
{
    RBNode * yp = nodep->left;
    nodep->left = yp->right;
    if (yp->right != RBTNIL)
        yp->right->parent = nodep;
    yp->parent = nodep->parent;
    if (nodep->parent==RBTNIL)
        self->root = yp;
    else if (nodep==nodep->parent->right)
        nodep->parent->right = yp;
    else
        nodep->parent->left = yp;
    yp->right = nodep;
    nodep->parent = yp;
}

// nodeはREDという前提
void 
_insert_fixup(BSTreeObject * self, RBNode * nodep)
{
    while (nodep->parent->color == RED)
    {
        if (nodep->parent == nodep->parent->parent->left)
        {
            RBNode * yp = nodep->parent->parent->right;
            if (yp->color == RED)
            {
                nodep->parent->color = BLACK;
                yp->color = BLACK;
                nodep->parent->parent->color = RED;
                nodep = nodep->parent->parent;
            }
            else 
            {
                if (nodep == nodep->parent->right)
                {
                    nodep = nodep->parent;
                    _left_rotate(self, nodep);
                }
                else
                {
                    nodep->parent->color = BLACK;
                    nodep->parent->parent->color = RED;
                    _right_rotate(self, nodep->parent->parent);
                }
            }
        }
        else
        {
            RBNode * yp = nodep->parent->parent->left;
            if (yp->color == RED)
            {
                nodep->parent->color = BLACK;
                yp->color = BLACK;
                nodep->parent->parent->color = RED;
                nodep = nodep->parent->parent;
            }
            else 
            {
                if (nodep == nodep->parent->left)
                {
                    nodep = nodep->parent;
                    _right_rotate(self, nodep);
                }
                else
                {
                    nodep->parent->color = BLACK;
                    nodep->parent->parent->color = RED;
                    _left_rotate(self, nodep->parent->parent);
                }
            }
        }
    }
    self->root->color = BLACK;
}

// uを除去してその場所にvを移植する
void 
_transplant(BSTreeObject * self, RBNode * nodeUp, RBNode * nodeVp)
{
    if (nodeUp->parent == RBTNIL)
        self->root = nodeVp;
    else if (nodeUp == nodeUp->parent->left)
        nodeUp->parent->left = nodeVp;
    else
        nodeUp->parent->right = nodeVp;
    nodeVp->parent = nodeUp->parent;
}

void
_delete_fixup(BSTreeObject * self, RBNode * nodep)
{
    while (nodep!=self->root && nodep->color==BLACK)
    {
        if (nodep==nodep->parent->left)
        {
            RBNode * wp = nodep->parent->right;
            if (wp->color==RED)
            {
                wp->color = BLACK;
                nodep->parent->color = RED;
                _left_rotate(self, nodep->parent);
                wp = nodep->parent->right;
            }
            if (wp->left->color==BLACK && wp->right->color==BLACK)
            {
                wp->color = RED;
                nodep = nodep->parent;
            }
            else 
            {
                if (wp->right->color==BLACK)
                {
                    wp->left->color = BLACK;
                    wp->color = RED;
                    _right_rotate(self, wp);
                    wp = nodep->parent->right;
                }
                else
                {
                    wp->color = nodep->parent->color;
                    nodep->parent->color = BLACK;
                    wp->right->color = BLACK;
                    _left_rotate(self, nodep->parent);
                    nodep = self->root;
                }
            }
        }
        else
        {
            RBNode * wp = nodep->parent->left;
            if (wp->color==RED)
            {
                wp->color = BLACK;
                nodep->parent->color = RED;
                _right_rotate(self, nodep->parent);
                wp = nodep->parent->left;
            }
            if (wp->right->color==BLACK && wp->left->color==BLACK)
            {
                wp->color = RED;
                nodep = nodep->parent;
            }
            else 
            {
                if (wp->left->color==BLACK)
                {
                    wp->right->color = BLACK;
                    wp->color = RED;
                    _left_rotate(self, wp);
                    wp = nodep->parent->left;
                }
                else
                {
                    wp->color = nodep->parent->color;
                    nodep->parent->color = BLACK;
                    wp->left->color = BLACK;
                    _right_rotate(self, nodep->parent);
                    nodep = self->root;
                }
            }
        }
    }
    nodep->color = BLACK;
}

static PyMemberDef bstree_class_members[] = 
{
    {"size", T_LONG, offsetof(BSTreeObject, size), READONLY},
    {NULL}
};

static PyMethodDef bstree_class_methods[] = 
{
    {"insert", (PyCFunction)bstree_insert, METH_VARARGS, "insert an integer"},
    {"delete", (PyCFunction)bstree_delete, METH_VARARGS, "delete an integer"},
    {"search", (PyCFunction)bstree_search, METH_VARARGS, "search an integer"},
    {"to_list", (PyCFunction)bstree_list, METH_VARARGS, "list in order"},
    {"print", (PyCFunction)bstree_print, METH_VARARGS, "print in order"},
    {"min", (PyCFunction)bstree_min, METH_NOARGS, "get a minimum value"},
    {"max", (PyCFunction)bstree_max, METH_NOARGS, "get a maximum value"},
    {0, NULL}
};

static PyType_Slot bstreeType_slots[] =
{
    {Py_tp_methods, bstree_class_methods},
    {Py_tp_init, (initproc)bstree_init},
    {Py_tp_members, bstree_class_members},
    {0, 0},
};

// classの定義
static PyType_Spec bstreeType_spec =
{
    .name = "bstree.BSTree",
    .basicsize = sizeof(BSTreeObject),
    // .itemsize = 0,
    .flags = Py_TPFLAGS_DEFAULT,
    .slots = bstreeType_slots,
};


// スロットの定義
// ここでBSTreeクラスをモジュールに登録している
static int
bstree_exec(PyObject * module)
{
    PyObject * type;
    type = PyType_FromSpec(&bstreeType_spec);
    if (!type) 
    {
        Py_DECREF(module);
        return -1;
    }
    if (PyModule_AddObject(module, "BSTree", type))
    {
        Py_DECREF(type);
        Py_DECREF(module);
        return -1;
    }
    return 0;
}
//　スロットの登録
static PyModuleDef_Slot bstree_module_slots[] = 
{
    {Py_mod_exec, bstree_exec},
    {0, NULL},
};


// 個々のモジュール関数の定義
static PyObject * bstree_testfunc1(PyObject * module)
{
    return NULL;
}
static PyObject * bstree_testfunc2(PyObject * module)
{
    return NULL;
}
// モジュール関数の登録
static PyMethodDef bstree_module_methods[] = 
{
    {"testfunc1", (PyCFunction)bstree_testfunc1, METH_VARARGS, "doc for testfunc1"},
    {"testfunc2", (PyCFunction)bstree_testfunc2, METH_VARARGS, "doc for testfunc2"},
    {NULL, NULL, 0, NULL},
};

// モジュールの定義
static struct PyModuleDef bstree_def = 
{
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "bstree",
    .m_doc = "document about bstree module",
    .m_size = 0,
    .m_methods = bstree_module_methods,
    .m_slots = bstree_module_slots,
};

// 初期化
PyMODINIT_FUNC
PyInit_bstree(void)
{
    return PyModuleDef_Init(&bstree_def);
} 