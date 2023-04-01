#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#ifndef BSTREE_H
#define BSTREE_H

#include <stdio.h>
#include <stdlib.h>

// macro definition
#define BLACK (0)
#define RED (1)
#define RBTNIL (&sentinel)

typedef struct
{
    PyObject_HEAD struct rbnode *root;
    unsigned size;
    PyObject *captured;
} BSTreeObject;

typedef struct rbnode
{
    // key
    long key;
    unsigned long size;
    unsigned long count;
    char color;
    struct rbnode *parent;
    struct rbnode *left;
    struct rbnode *right;
} RBNode;

#endif // BSTREE_H

// private function declaration
RBNode *_create_node(long);
RBNode *_search(BSTreeObject *, long);
RBNode *_search_fixup(BSTreeObject *, long);
void _left_rotate(BSTreeObject *, RBNode *);
void _right_rotate(BSTreeObject *, RBNode *);
void _insert_fixup(BSTreeObject *, RBNode *);
void _update_size_when_deleted(BSTreeObject *, RBNode *);
void _decrease_node_size(RBNode *, RBNode *);
void _delete_fixup(BSTreeObject *, RBNode *);
void _transplant(BSTreeObject *, RBNode *, RBNode *);
PyObject *_list_in_order(RBNode *, PyObject *, int *);
RBNode *_get_min(RBNode *);
RBNode *_get_max(RBNode *);
RBNode *_get_next(RBNode *);
RBNode *_get_prev(RBNode *);
int _helper_smallest(RBNode *, unsigned long, long *);
int _helper_largest(RBNode *, unsigned long, long *);

// leaf node：every leaf is treated as the same node
RBNode sentinel =
    {
        .color = BLACK,
        .left = RBTNIL,
        .right = RBTNIL,
        .parent = NULL};

// method definiton
int bstree_init(BSTreeObject *self, PyObject *args)
{
    self->root = RBTNIL;
    self->size = 0;
    return 0;
}

static PyObject *
bstree_insert(BSTreeObject *self, PyObject *args)
{
    long key;
    if (!PyArg_ParseTuple(args, "l", &key))
    {
        return NULL;
    }
    // create a node first
    RBNode *nodep = _create_node(key);
    self->size += 1;

    RBNode *yp = RBTNIL;
    RBNode *xp = self->root;
    while (xp != RBTNIL)
    {
        xp->size += 1;
        yp = xp;
        if (nodep->key < xp->key)
            xp = xp->left;
        else if (nodep->key > xp->key)
            xp = xp->right;
        else
        {
            xp->count += 1;
            free(nodep);
            Py_RETURN_NONE;
        }
    }
    nodep->parent = yp;
    if (yp == RBTNIL)
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
bstree_delete(BSTreeObject *self, PyObject *args)
{
    long key;
    RBNode *nodep;

    if (!PyArg_ParseTuple(args, "l", &key))
        return NULL;

    if ((nodep = _search(self, key)) == RBTNIL)
        return NULL;

    self->size -= 1;
    _update_size_when_deleted(self, nodep);

    RBNode *yp = nodep;
    RBNode *xp;
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
    }
    if (y_original_color == BLACK)
        _delete_fixup(self, xp);
    free(nodep);
    Py_RETURN_NONE;
}

static PyObject *
bstree_search(BSTreeObject *self, PyObject *args)
{
    long key;
    if (!PyArg_ParseTuple(args, "l", &key))
        return NULL;

    if (_search(self, key) == RBTNIL)
        return Py_False;
    else
        return Py_True;
}

// return a list in ascending order
static PyObject *
bstree_list(BSTreeObject *self, PyObject *args)
{
    PyObject *list;
    int idx;
    idx = 0;
    list = PyList_New(self->size);
    RBNode *node = self->root;
    return _list_in_order(node, list, &idx);
}

static PyObject *
bstree_min(BSTreeObject *self, PyObject *args)
{
    RBNode *nodep = _get_min(self->root);
    if (nodep == RBTNIL)
        return NULL;
    return Py_BuildValue("l", nodep->key);
}

static PyObject *
bstree_max(BSTreeObject *self, PyObject *args)
{
    RBNode *nodep = _get_max(self->root);
    if (nodep == RBTNIL)
        return NULL;
    return Py_BuildValue("l", nodep->key);
}

static PyObject *
bstree_kth_smallest(BSTreeObject *self, PyObject *args)
{
    unsigned long k;
    int ret;
    long ans;
    if (!PyArg_ParseTuple(args, "|k", &k))
        return NULL;
    if (PyTuple_Size(args) == 0)
        k = 1;
    ret = _helper_smallest(self->root, k, &ans);
    if (ret == -1)
        return NULL;
    return Py_BuildValue("l", ans);
}

static PyObject *
bstree_kth_largest(BSTreeObject *self, PyObject *args)
{
    unsigned long k;
    int ret;
    long ans;
    if (!PyArg_ParseTuple(args, "|k", &k))
        return NULL;
    if (PyTuple_Size(args) == 0)
        k = 1;
    ret = _helper_largest(self->root, k, &ans);
    if (ret == -1)
        return NULL;
    return Py_BuildValue("l", ans);
}

static PyObject *
bstree_rank(BSTreeObject *self, PyObject *args)
{
    unsigned long _get_rank(RBNode *, long);
    long key;
    if (!PyArg_ParseTuple(args, "l", &key))
        return NULL;

    return Py_BuildValue("k", _get_rank(self->root, key));
}

static PyObject *
_list_in_order(RBNode *node, PyObject *list, int *pidx)
{
    if (node->left != RBTNIL)
        list = _list_in_order(node->left, list, pidx);

    for (int i = 0; i < node->count; i++)
        PyList_SET_ITEM(list, *pidx + i, PyLong_FromLong(node->key));
    *pidx += node->count;

    if (node->right != RBTNIL)
        list = _list_in_order(node->right, list, pidx);

    return list;
}

int _helper_smallest(RBNode *node, unsigned long k, long *ans)
{
    if (k > node->size)
    {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        return -1;
    }
    if (node == RBTNIL)
        return 0;
    if (k <= node->left->size)
        return _helper_smallest(node->left, k, ans);
    else if (node->left->size < k && k <= node->left->size + node->count)
    {
        *ans = node->key;
        return 0;
    }
    else
        return _helper_smallest(node->right, k - node->left->size - node->count, ans);
}

int _helper_largest(RBNode *node, unsigned long k, long *ans)
{
    if (k > node->size)
    {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        return -1;
    }
    if (node == RBTNIL)
        return 0;
    if (k <= node->right->size)
        return _helper_largest(node->right, k, ans);
    else if (node->right->size < k && k <= node->right->size + node->count)
    {
        *ans = node->key;
        return 0;
    }
    else
        return _helper_largest(node->left, k - node->right->size - node->count, ans);
}

unsigned long _get_rank(RBNode *node, long key)
{
    if (node == RBTNIL)
        return 0;
    if (key < node->key)
        return _get_rank(node->left, key);
    else if (key > node->key)
        return node->left->size + node->count + _get_rank(node->right, key);
    else
        return node->left->size;
}

void _update_size_when_deleted(BSTreeObject *self, RBNode *target)
{
    RBNode *node = self->root;
    _decrease_node_size(node, target);
}

void _decrease_node_size(RBNode *node, RBNode *target)
{
    if (target == RBTNIL)
        return;
    node->size -= 1;
    if (target->key < node->key)
        _decrease_node_size(node->left, target);
    else if (target->key > node->key)
        _decrease_node_size(node->right, target);
    else
        return;
}

// get the node which key is k.
// If not exist, get RBTNIL.
RBNode *_search(BSTreeObject *self, long k)
{
    RBNode *zp = self->root;
    while (zp != RBTNIL && k != zp->key)
    {
        if (k < zp->key)
            zp = zp->left;
        else
            zp = zp->right;
    }
    return zp;
}

// get the node which key is k.
// If not exist, get RBTNIL.
RBNode *_search_fixup(BSTreeObject *self, long k)
{
    RBNode *zp = self->root;
    if (zp == RBTNIL)
        return RBTNIL;
    while (k != zp->key)
    {
        if (k < zp->key && zp->left != RBTNIL)
            zp = zp->left;
        else if (k > zp->key && zp->right != RBTNIL)
            zp = zp->right;
        else
            break;
    }
    return zp;
}

RBNode *_create_node(long key)
{
    RBNode *nodep = malloc(sizeof(RBNode));
    if (nodep == NULL)
        return NULL;
    nodep->key = key;
    nodep->size = 1;
    nodep->count = 1;
    nodep->parent = RBTNIL;
    nodep->left = RBTNIL;
    nodep->right = RBTNIL;
    return nodep;
}

// get the min value of the tree which root is nodep
RBNode *_get_min(RBNode *nodep)
{
    RBNode *zp = nodep;
    while (zp->left != RBTNIL)
        zp = zp->left;
    return zp;
}

// get the max value of the tree which root is nodep
RBNode *_get_max(RBNode *nodep)
{
    RBNode *zp = nodep;
    while (zp->right != RBTNIL)
        zp = zp->right;
    return zp;
}

static PyObject *
bstree_next(BSTreeObject *self, PyObject *args)
{
    RBNode *_get_next(RBNode *);
    long k;
    if (!PyArg_ParseTuple(args, "l", &k))
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    RBNode *nodep = _search_fixup(self, k);
    if (nodep == RBTNIL)
        Py_RETURN_NONE;
    else if (nodep->key > k)
        return Py_BuildValue("l", nodep->key);
    else
    {
        RBNode *nextp = _get_next(nodep);
        if (nextp != RBTNIL)
            return Py_BuildValue("l", _get_next(nodep)->key);
        else
            Py_RETURN_NONE;
    }
}

static PyObject *
bstree_prev(BSTreeObject *self, PyObject *args)
{
    long k;
    if (!PyArg_ParseTuple(args, "l", &k))
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    RBNode *nodep = _search_fixup(self, k);
    if (nodep == RBTNIL)
        Py_RETURN_NONE;
    else if (nodep->key < k)
        return Py_BuildValue("l", nodep->key);
    else
    {
        RBNode *nextp = _get_prev(nodep);
        if (nextp != RBTNIL)
            return Py_BuildValue("l", _get_prev(nodep)->key);
        else
            Py_RETURN_NONE;
    }
}

// get the value of node which is next to nodep
// if no node, return RBTNIL
// assuming that nodep is in the tree
RBNode *_get_next(RBNode *nodep)
{
    if (nodep->right != RBTNIL)
        return _get_min(nodep->right);

    RBNode *pp = nodep->parent;
    while (pp != RBTNIL && nodep == pp->right)
    {
        nodep = pp;
        pp = nodep->parent;
    }
    return pp;
}

// assuming that nodep is in the tree
RBNode *_get_prev(RBNode *nodep)
{
    if (nodep->left != RBTNIL)
        return _get_max(nodep->left);
    RBNode *pp = nodep->parent;
    while (pp != RBTNIL && nodep == pp->left)
    {
        nodep = pp;
        pp = nodep->parent;
    }
    return pp;
}

static void
_left_rotate(BSTreeObject *self, RBNode *nodep)
{
    RBNode *yp = nodep->right;
    // update size
    yp->size = nodep->size;
    nodep->size = nodep->left->size + nodep->count + yp->left->size;

    nodep->right = yp->left;
    if (yp->left != RBTNIL)
        yp->left->parent = nodep;
    yp->parent = nodep->parent;
    if (nodep->parent == RBTNIL)
        self->root = yp;
    else if (nodep == nodep->parent->left)
        nodep->parent->left = yp;
    else
        nodep->parent->right = yp;
    yp->left = nodep;
    nodep->parent = yp;
}

static void
_right_rotate(BSTreeObject *self, RBNode *nodep)
{
    RBNode *yp = nodep->left;
    // update size
    yp->size = nodep->size;
    nodep->size = nodep->right->size + nodep->count + yp->right->size;

    nodep->left = yp->right;
    if (yp->right != RBTNIL)
        yp->right->parent = nodep;
    yp->parent = nodep->parent;
    if (nodep->parent == RBTNIL)
        self->root = yp;
    else if (nodep == nodep->parent->right)
        nodep->parent->right = yp;
    else
        nodep->parent->left = yp;
    yp->right = nodep;
    nodep->parent = yp;
}

// assuming that nodep is in the tree
void _insert_fixup(BSTreeObject *self, RBNode *nodep)
{
    while (nodep->parent->color == RED)
    {
        if (nodep->parent == nodep->parent->parent->left)
        {
            RBNode *yp = nodep->parent->parent->right;
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
            RBNode *yp = nodep->parent->parent->left;
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

// remove u, and transplant v where u was
void _transplant(BSTreeObject *self, RBNode *nodeUp, RBNode *nodeVp)
{
    if (nodeUp->parent == RBTNIL)
        self->root = nodeVp;
    else if (nodeUp == nodeUp->parent->left)
        nodeUp->parent->left = nodeVp;
    else
        nodeUp->parent->right = nodeVp;
    nodeVp->parent = nodeUp->parent;
}

void _delete_fixup(BSTreeObject *self, RBNode *nodep)
{
    while (nodep != self->root && nodep->color == BLACK)
    {
        if (nodep == nodep->parent->left)
        {
            RBNode *wp = nodep->parent->right;
            if (wp->color == RED)
            {
                wp->color = BLACK;
                nodep->parent->color = RED;
                _left_rotate(self, nodep->parent);
                wp = nodep->parent->right;
            }
            if (wp->left->color == BLACK && wp->right->color == BLACK)
            {
                wp->color = RED;
                nodep = nodep->parent;
            }
            else
            {
                if (wp->right->color == BLACK)
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
            RBNode *wp = nodep->parent->left;
            if (wp->color == RED)
            {
                wp->color = BLACK;
                nodep->parent->color = RED;
                _right_rotate(self, nodep->parent);
                wp = nodep->parent->left;
            }
            if (wp->right->color == BLACK && wp->left->color == BLACK)
            {
                wp->color = RED;
                nodep = nodep->parent;
            }
            else
            {
                if (wp->left->color == BLACK)
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
        {NULL}};

static PyMethodDef bstree_class_methods[] =
    {
        {"insert", (PyCFunction)bstree_insert, METH_VARARGS, "insert an integer"},
        {"delete", (PyCFunction)bstree_delete, METH_VARARGS, "delete an integer"},
        {"search", (PyCFunction)bstree_search, METH_VARARGS, "search an integer"},
        {"to_list", (PyCFunction)bstree_list, METH_VARARGS, "list in order"},
        {"next_to", (PyCFunction)bstree_next, METH_VARARGS, "get a next value"},
        {"prev_to", (PyCFunction)bstree_prev, METH_VARARGS, "get a prev value"},
        {"min", (PyCFunction)bstree_min, METH_NOARGS, "get a minimum value"},
        {"max", (PyCFunction)bstree_max, METH_NOARGS, "get a maximum value"},
        {"kth_smallest", (PyCFunction)bstree_kth_smallest, METH_VARARGS, "get a kth smallest value"},
        {"kth_largest", (PyCFunction)bstree_kth_largest, METH_VARARGS, "get a kth largest value"},
        {"rank", (PyCFunction)bstree_rank, METH_VARARGS, "get a rank of parameter"},
        {0, NULL}};

static PyType_Slot bstreeType_slots[] =
    {
        {Py_tp_methods, bstree_class_methods},
        {Py_tp_init, (initproc)bstree_init},
        {Py_tp_members, bstree_class_members},
        {0, 0},
};

// class definition
static PyType_Spec bstreeType_spec =
    {
        .name = "bstree.BSTree",
        .basicsize = sizeof(BSTreeObject),
        // .itemsize = 0,
        .flags = Py_TPFLAGS_DEFAULT,
        .slots = bstreeType_slots,
};

// slot definition
// registering BSTree class to bstree module
static int
bstree_exec(PyObject *module)
{
    PyObject *type;
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
// 　register slot
static PyModuleDef_Slot bstree_module_slots[] =
    {
        {Py_mod_exec, bstree_exec},
        {0, NULL},
};

// module function definition
// not implemented yet
static PyObject *bstree_testfunc1(PyObject *module)
{
    return NULL;
}
static PyObject *bstree_testfunc2(PyObject *module)
{
    return NULL;
}
// register module functions
static PyMethodDef bstree_module_methods[] =
    {
        {"testfunc1", (PyCFunction)bstree_testfunc1, METH_VARARGS, "doc for testfunc1"},
        {"testfunc2", (PyCFunction)bstree_testfunc2, METH_VARARGS, "doc for testfunc2"},
        {NULL, NULL, 0, NULL},
};

// module definition
static struct PyModuleDef bstree_def =
    {
        .m_base = PyModuleDef_HEAD_INIT,
        .m_name = "bstree",
        .m_doc = "document about bstree module",
        .m_size = 0,
        .m_methods = bstree_module_methods,
        .m_slots = bstree_module_slots,
};

// initialize module
PyMODINIT_FUNC
PyInit_bstree(void)
{
    return PyModuleDef_Init(&bstree_def);
}