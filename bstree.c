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

#define COMPARE_ERR INT_MIN
#define INC_REF 1
#define KEEP_REF 0
#define DEC_REF -1

typedef struct objnode
{
    PyObject *obj;
    struct objnode *next;
} ObjNode;

typedef struct rbnode
{
    // the key of the object to compare
    PyObject *key;
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

// compare a with b and return -1, 0, 1
typedef int (*CompareOperator)(PyObject *, PyObject *);

// whether tree holds duplicated key or not
// if so, node count will increase.
enum IsDup
{
    NO_DUP,
    DUP
};

typedef struct
{
    PyObject_HEAD RBNode *root;
    unsigned long size;
    enum IsDup is_dup;
    CompareOperator ope;
    PyObject *keyfunc;
    PyObject *captured;
} BSTreeObject;

#endif // BSTREE_H

// private function declaration
RBNode *_create_rbnode(PyObject *, PyObject *);
void _delete_rbnode(RBNode *);
int _add_obj_to_rbnode(PyObject *, RBNode *);
int _delete_obj_from_rbnode(RBNode *);
RBNode *_search(PyObject *, RBNode *, CompareOperator);
RBNode *_search_fixup(PyObject *, RBNode *, CompareOperator);
void _left_rotate(BSTreeObject *, RBNode *);
void _right_rotate(BSTreeObject *, RBNode *);
void _insert_fixup(BSTreeObject *, RBNode *);
void _update_size(BSTreeObject *, RBNode *);
void _delete_fixup(BSTreeObject *, RBNode *);
void _transplant(BSTreeObject *, RBNode *, RBNode *);
PyObject *_list_in_order(RBNode *, PyObject *, int *, char);
int _add_counter(RBNode *, PyObject *);
void _delete_all_rbnodes(RBNode *);

RBNode *_get_min(RBNode *);
RBNode *_get_max(RBNode *);
RBNode *_get_next(RBNode *);
RBNode *_get_prev(RBNode *);
long _get_rank(PyObject *, RBNode *, CompareOperator);
int _helper_smallest(RBNode *, unsigned long, PyObject **);
int _helper_largest(RBNode *, unsigned long, PyObject **);

int _lt_long(PyObject *, PyObject *);
int _lt_double(PyObject *, PyObject *);
int _lt_obj(PyObject *, PyObject *);
int _compare(PyObject *, PyObject *, CompareOperator);
int _can_be_handled_as_c_long(PyObject *);

// every leaf is treated as the same node
// left, right, parent can take an arbitrary value
RBNode sentinel =
    {
        .color = BLACK,
        .left = RBTNIL,
        .right = RBTNIL,
        .parent = NULL,
        .size = 0};

// class constructor
// has to return 0 on success, -1 on failure
static int
bstree_init(BSTreeObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *dup = Py_False;
    PyObject *func = Py_None;
    static char *kwlists[] = {"dup", "key", NULL};

    // | is optional
    // O is PyObject
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OO", kwlists, &dup, &func))
    {
        return -1;
    }

    if (!PyBool_Check(dup))
    {
        PyErr_SetString(PyExc_TypeError, "'dup' must be a boolean (True or False)");
        return -1;
    }

    if (func != Py_None && !PyCallable_Check(func))
    {
        PyErr_SetString(PyExc_TypeError, "key must be callable or None");
        return -1;
    }
    self->keyfunc = func;
    Py_XINCREF(func);
    self->root = RBTNIL;
    self->size = 0;
    self->is_dup = dup == Py_False ? NO_DUP : DUP;
    self->ope = NULL;
    return 0;
}

// clear the tree but keep the conf of dup
static PyObject *
bstree_clear(BSTreeObject *self, PyObject *args)
{
    if (self->root != RBTNIL)
        _delete_all_rbnodes(self->root);

    self->root = RBTNIL;
    self->size = 0;
    self->ope = NULL;
    Py_RETURN_NONE;
}

int _lt_long(PyObject *a, PyObject *b)
{
    long value_a = PyLong_AsLong(a);
    long value_b = PyLong_AsLong(b);

    if (value_a == -1 && PyErr_Occurred())
        return COMPARE_ERR;
    if (value_b == -1 && PyErr_Occurred())
        return COMPARE_ERR;

    return value_a < value_b ? 1 : 0;
}

int _lt_double(PyObject *a, PyObject *b)
{
    double value_a = PyFloat_AsDouble(a);
    double value_b = PyFloat_AsDouble(b);

    // a->key or b->key might be python int type
    if (value_a == -1 && PyErr_Occurred())
    {
        if ((value_a = (double)PyLong_AsLong(a)) != -1)
            PyErr_Clear();
        else
            return COMPARE_ERR;
    }
    if (value_b == -1 && PyErr_Occurred())
    {
        if ((value_b = (double)PyLong_AsLong(b)) != -1)
            PyErr_Clear();
        else
            return COMPARE_ERR;
    }
    return value_a < value_b ? 1 : 0;
}

// if a < b return 1, elif a >= b return 0, else return COMPARE_ERR.
int _lt_obj(PyObject *a, PyObject *b)
{
    PyObject *lt_name = PyUnicode_InternFromString("__lt__");
    PyObject *lt_result = PyObject_CallMethodObjArgs(a, lt_name, b, NULL);
    if (lt_result != NULL && PyBool_Check(lt_result))
    {
        if (PyObject_IsTrue(lt_result))
        {
            Py_DECREF(lt_result);
            return 1; // when a < b
        }
        else
        {
            Py_DECREF(lt_result);
            return 0; // when a >= b
        }
    }

    PyObject *gt_name = PyUnicode_InternFromString("__gt__");
    PyObject *gt_result = PyObject_CallMethodObjArgs(b, gt_name, a, NULL);
    if (gt_result != NULL && PyBool_Check(gt_result))
    {
        if (PyObject_IsTrue(gt_result))
        {
            Py_DECREF(gt_result);
            return 1; // when b > a
        }
        else
        {
            Py_DECREF(gt_result);
            return 0; // when b <= a
        }
    }
    PyErr_SetString(PyExc_TypeError, "Compare Error");
    return COMPARE_ERR;
}

// if a < b return 1, elif a > b return -1, elif a == b return 0 else return COMPARE_ERR
int _compare(PyObject *a, PyObject *b, CompareOperator comp)
{
    int a_comp_b = comp(a, b);
    int b_comp_a = comp(b, a);
    if (a_comp_b == COMPARE_ERR || b_comp_a == COMPARE_ERR)
    {
        return COMPARE_ERR;
    }
    if (a_comp_b == 0 && b_comp_a == 0)
    {
        return 0;
    }
    else if (a_comp_b == 1 && b_comp_a == 0)
    {
        return 1;
    }
    else if (a_comp_b == 0 && b_comp_a == 1)
    {
        return -1;
    }
    else
    {
        return COMPARE_ERR;
    }
}

// check if the python object can be handled as a long type in c
int _can_be_handled_as_c_long(PyObject *obj)
{
    if (PyLong_AsLong(obj) == -1 && PyErr_Occurred())
    {
        PyErr_Clear();
        return 0;
    }
    return 1;
}

// caution: obj is a pointer to python tuple
static PyObject *
bstree_insert(BSTreeObject *self, PyObject *args)
{
    // fetch the first arg
    if (!PyTuple_Check(args))
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    if (PyTuple_Size(args) != 1)
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    PyObject *obj = PyTuple_GetItem(args, 0);

    // if the object is Nonetype, raise NotImplementedError
    if (obj == Py_None)
    {
        PyErr_SetString(PyExc_TypeError, "NoneType is not supported");
        return NULL;
    }

    PyObject *key = self->keyfunc == Py_None ? obj : PyObject_CallFunctionObjArgs(self->keyfunc, obj, NULL);
    if (key == NULL)
        return NULL;

    // validate the object and determine the comparison operator
    if (self->ope == NULL)
    {
        // python3 always treat any large or small integer as int type
        // but c can not handle it as long type if its value is too large or too small
        if (PyLong_Check(key))
        {
            if (_can_be_handled_as_c_long(key))
            {
                self->ope = _lt_long;
                // printf("comp changed from null to _lt_long\n");
            }
            else
            {
                self->ope = _lt_double;
                // printf("comp changed from null to _lt_double\n");
            }
        }
        else if (PyFloat_Check(key))
        {
            self->ope = _lt_double;
            // printf("comp changed from null to _lt_double\n");
        }
        else
        {
            self->ope = _lt_obj;
            // printf("comp changed from null to _lt_obj\n");
        }
    }
    else if (self->ope == _lt_long)
    {
        if (PyLong_Check(key))
        {
            if (!_can_be_handled_as_c_long(key))
            {
                self->ope = _lt_double;
                // printf("comp changed from _lt_long to _lt_double\n");
            }
        }
        else if (PyFloat_Check(key))
        {
            self->ope = _lt_double;
            // printf("comp changed from _lt_long to _lt_double\n");
        }
        else
        {
            self->ope = _lt_obj;
            // printf("comp changed from _lt_long to _lt_obj\n");
        }
    }
    else if (self->ope == _lt_double)
    {
        if (!PyFloat_Check(key))
        {
            if (PyLong_Check(key))
            {
                ;
            }
            else
            {
                self->ope = _lt_obj;
                // printf("comp changed from _lt_double to _lt_obj\n");
            }
        }
    }

    RBNode *yp = RBTNIL;
    RBNode *xp = self->root;
    while (xp != RBTNIL)
    {
        yp = xp;
        int comp_with_x;
        if ((comp_with_x = _compare(key, xp->key, self->ope)) == COMPARE_ERR)
        {
            PyErr_SetString(PyExc_TypeError, "Comparison Error");
            return NULL;
        }
        if (comp_with_x > 0)
        {
            xp = xp->left;
        }
        else if (comp_with_x < 0)
        {
            xp = xp->right;
        }
        // if the node already exists, just increase the node count and
        // the whole tree size, only when dup is true.
        else
        {
            if (self->is_dup == NO_DUP)
            {
                Py_RETURN_FALSE;
            }
            else
            {
                if (_add_obj_to_rbnode(obj, xp) == -1)
                {
                    PyErr_SetString(PyExc_TypeError, "Add Object Error");
                    return NULL;
                }
                self->size += 1;
                _update_size(self, xp);
                Py_RETURN_TRUE;
            }
        }
    }
    // if the node doesn't exist, just increase the whole tree size.
    RBNode *nodep = _create_rbnode(obj, self->keyfunc);
    if (nodep == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Create Node Error");
        return NULL;
    }
    self->size += 1;
    nodep->parent = yp;
    int comp_with_y;
    if (yp == RBTNIL)
        self->root = nodep;
    else if ((comp_with_y = _compare(key, yp->key, self->ope)) == COMPARE_ERR)
    {
        _delete_rbnode(nodep);
        PyErr_SetString(PyExc_TypeError, "Comparison Error");
        return NULL;
    }
    else if (comp_with_y > 0)
        yp->left = nodep;
    else
        yp->right = nodep;
    _update_size(self, nodep);
    nodep->color = RED;
    _insert_fixup(self, nodep);
    Py_RETURN_NONE;
}

// caution: args is a pointer to python tuple
static PyObject *
bstree_delete(BSTreeObject *self, PyObject *args)
{
    RBNode *nodep;

    // fetch the first arg
    if (!PyTuple_Check(args))
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    // if len(args) != 1 type error
    if (PyTuple_Size(args) != 1)
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    PyObject *obj = PyTuple_GetItem(args, 0);
    PyObject *key = self->keyfunc == Py_None ? obj : PyObject_CallFunctionObjArgs(self->keyfunc, obj, NULL);
    if (key == NULL)
        return NULL;

    nodep = _search(key, self->root, self->ope);
    if (nodep == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    if (nodep == RBTNIL)
    {
        PyErr_SetString(PyExc_ValueError, "bstree.remove(x): x not in tree");
        return NULL;
    }
    self->size -= 1;

    RBNode *yp = nodep;
    RBNode *xp, *wp;
    char y_original_color = yp->color;

    if (nodep->count > 1)
    {
        if (_delete_obj_from_rbnode(nodep) == -1)
        {
            PyErr_SetString(PyExc_TypeError, "Delete Object Error");
            return NULL;
        }
        _update_size(self, nodep);
        Py_RETURN_TRUE;
    }
    if (nodep->left == RBTNIL && nodep->right == RBTNIL)
    {
        xp = RBTNIL;
        _transplant(self, nodep, xp);
        _update_size(self, nodep->parent);
    }
    else if (nodep->left == RBTNIL)
    {
        xp = nodep->right;
        _transplant(self, nodep, xp);
        _update_size(self, xp);
    }
    else if (nodep->right == RBTNIL)
    {
        xp = nodep->left;
        _transplant(self, nodep, xp);
        _update_size(self, xp);
    }
    else
    {
        yp = _get_min(nodep->right);
        y_original_color = yp->color;
        // xp could be RBTNIL
        xp = yp->right;
        wp = yp->parent;
        if (yp->parent == nodep)
            xp->parent = yp;
        else
        {
            _transplant(self, yp, xp);
            // making a subtree which root is yp
            yp->right = nodep->right;
            yp->right->parent = yp;
            yp->parent = RBTNIL;
            if (xp != RBTNIL)
                _update_size(self, xp);
            else
                _update_size(self, wp);
        }
        _transplant(self, nodep, yp);
        yp->left = nodep->left;
        yp->left->parent = yp;
        yp->color = nodep->color;
        _update_size(self, yp);
    }
    if (y_original_color == BLACK)
        _delete_fixup(self, xp);
    _delete_rbnode(nodep);
    Py_RETURN_NONE;
}

// check if the key exists in the tree
// caution: obj is a pointer to python tuple
static PyObject *
bstree_has(BSTreeObject *self, PyObject *args)
{
    // fetch the first arg
    if (!PyTuple_Check(args))
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    if (PyTuple_Size(args) != 1)
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    PyObject *obj = PyTuple_GetItem(args, 0);
    PyObject *key = self->keyfunc == Py_None ? obj : PyObject_CallFunctionObjArgs(self->keyfunc, obj, NULL);
    if (key == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Comparison Error");
        return NULL;
    }
    RBNode *nodep = _search(key, self->root, self->ope);
    if (nodep == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    if (nodep == RBTNIL)
        return Py_False;
    else
        return Py_True;
}

// return a list of objects in ascending order
static PyObject *
bstree_list(BSTreeObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *rev_obj = NULL;
    static char *kwlists[] = {"reverse", NULL};
    char is_reverse = 0;

    // the number of arguments are 0 or 1、keyarg is "reverse" only
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", kwlists, &rev_obj))
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    if (rev_obj != NULL)
    {
        if (!PyBool_Check(rev_obj))
        {
            PyErr_SetString(PyExc_TypeError, "Argument must be a boolean value");
            return NULL;
        }
        if (rev_obj == Py_True)
        {
            is_reverse = 1;
        }
    }
    int idx = 0;
    PyObject *list = PyList_New(self->size);
    RBNode *node = self->root;
    return _list_in_order(node, list, &idx, is_reverse);
}

static PyObject *
bstree_counter(BSTreeObject *self, PyObject *args)
{
    PyObject *dict = PyDict_New();
    RBNode *node = self->root;
    if (node == RBTNIL)
        return dict;
    if (_add_counter(node, dict) == -1)
    {
        PyErr_SetString(PyExc_TypeError, "Counter Error");
        return NULL;
    }
    return dict;
}

static PyObject *
bstree_min(BSTreeObject *self, PyObject *args)
{
    RBNode *nodep = _get_min(self->root);
    if (nodep == RBTNIL)
    {
        PyErr_SetString(PyExc_ValueError, " Cannot determine minimum: the tree is empty");
        return NULL;
    }
    return Py_BuildValue("O", nodep->obj_list->obj);
}

static PyObject *
bstree_max(BSTreeObject *self, PyObject *args)
{
    RBNode *nodep = _get_max(self->root);
    if (nodep == RBTNIL)
    {
        PyErr_SetString(PyExc_ValueError, " Cannot determine maximum: the tree is empty");
        return NULL;
    }
    return Py_BuildValue("O", nodep->obj_list->obj);
}

static PyObject *
bstree_kth_smallest(BSTreeObject *self, PyObject *args)
{
    unsigned long k;
    int ret;
    PyObject *ans = NULL;
    if (!PyArg_ParseTuple(args, "|k", &k))
    {
        PyErr_SetString(PyExc_TypeError, "Invalid argument: expected an optional non-negative integer");
        return NULL;
    }
    if (PyTuple_Size(args) == 0)
        k = 1;
    ret = _helper_smallest(self->root, k, &ans); // pointer to ans
    if (ret == -1)
    {
        PyErr_SetString(PyExc_ValueError, "k must be between 1 and the number of elements in the tree");
        return NULL;
    }
    return Py_BuildValue("O", ans);
}

static PyObject *
bstree_kth_largest(BSTreeObject *self, PyObject *args)
{
    unsigned long k;
    int ret;
    PyObject *ans = NULL;
    if (!PyArg_ParseTuple(args, "|k", &k))
    {
        PyErr_SetString(PyExc_TypeError, "Invalid argument: expected an optional non-negative integer");
        return NULL;
    }
    if (PyTuple_Size(args) == 0)
        k = 1;
    ret = _helper_largest(self->root, k, &ans);
    if (ret == -1)
    {
        PyErr_SetString(PyExc_ValueError, "k must be between 1 and the number of elements in the tree");
        return NULL;
    }
    return Py_BuildValue("O", ans);
}

/// equivalent to (sort(); bisect_left();)
static PyObject *
bstree_rank(BSTreeObject *self, PyObject *args)
{
    if (PyTuple_Size(args) != 1)
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    PyObject *obj = PyTuple_GetItem(args, 0);
    PyObject *key = self->keyfunc == Py_None ? obj : PyObject_CallFunctionObjArgs(self->keyfunc, obj, NULL);
    if (key == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Comparison Error");
        return NULL;
    }
    long rank = _get_rank(key, self->root, self->ope);
    if (rank < 0)
    {
        PyErr_SetString(PyExc_TypeError, "Comparison Error");
        return NULL;
    }
    return Py_BuildValue("k", rank);
}

PyObject *
_list_in_order(RBNode *node, PyObject *list, int *pidx, char is_reverse)
{
    if (is_reverse == 0)
    {
        if (node->left != RBTNIL)
            list = _list_in_order(node->left, list, pidx, is_reverse);

        ObjNode *current = node->obj_list;
        while (current != NULL)
        {
            PyList_SET_ITEM(list, *pidx, Py_BuildValue("O", current->obj));
            current = current->next;
            *pidx += 1;
        }

        if (node->right != RBTNIL)
            list = _list_in_order(node->right, list, pidx, is_reverse);
    }
    else
    {
        if (node->right != RBTNIL)
            list = _list_in_order(node->right, list, pidx, is_reverse);

        ObjNode *current = node->obj_list;
        while (current != NULL)
        {
            PyList_SET_ITEM(list, *pidx, Py_BuildValue("O", current->obj));
            current = current->next;
            *pidx += 1;
        }

        if (node->left != RBTNIL)
            list = _list_in_order(node->left, list, pidx, is_reverse);
    }
    return list;
}

void _delete_all_rbnodes(RBNode *node)
{
    if (node->left != RBTNIL)
        _delete_all_rbnodes(node->left);
    if (node->right != RBTNIL)
        _delete_all_rbnodes(node->right);
    _delete_rbnode(node);
}

int _add_counter(RBNode *node, PyObject *dict)
{
    if (node->left != RBTNIL && _add_counter(node->left, dict) == -1)
        return -1;

    // check if node->key is hashable
    if (!PyObject_HasAttrString(node->key, "__hash__"))
        return -1;

    if (PyDict_SetItem(dict, Py_BuildValue("O", node->key), Py_BuildValue("k", node->count)) == -1)
        return -1;

    if (node->right != RBTNIL && _add_counter(node->right, dict) == -1)
        return -1;

    return 0;
}

// search for the kth smallest object from the root and assign it to ans
int _helper_smallest(RBNode *rootp, unsigned long k, PyObject **ans)
{
    if (k < 1 || k > rootp->size)
        return -1;
    if (rootp == RBTNIL)
        return 0;
    if (k <= rootp->left->size)
        return _helper_smallest(rootp->left, k, ans);
    else if (rootp->left->size < k && k <= rootp->left->size + rootp->count)
    {
        *ans = rootp->obj_list->obj; // update ans
        return 0;
    }
    else
        return _helper_smallest(rootp->right, k - rootp->left->size - rootp->count, ans);
}

// search for the kth largest object from the root and assign it to ans
int _helper_largest(RBNode *rootp, unsigned long k, PyObject **ans)
{
    if (k < 1 || k > rootp->size)
        return -1;
    if (rootp == RBTNIL)
        return 0;
    if (k <= rootp->right->size)
        return _helper_largest(rootp->right, k, ans);
    else if (rootp->right->size < k && k <= rootp->right->size + rootp->count)
    {
        *ans = rootp->obj_list->obj;
        return 0;
    }
    else
        return _helper_largest(rootp->left, k - rootp->right->size - rootp->count, ans);
}

long _get_rank(PyObject *key, RBNode *nodep, CompareOperator ope)
{
    if (nodep == RBTNIL)
    {
        return 0;
    }
    int comp_with_x = _compare(key, nodep->key, ope);
    if (comp_with_x == COMPARE_ERR)
    {
        PyErr_SetString(PyExc_TypeError, "Comparison Error");
        // [TODO] should not return NULL, what about 0?
        return -1;
    }
    if (comp_with_x > 0)
    {
        return _get_rank(key, nodep->left, ope);
    }
    else if (comp_with_x < 0)
    {
        long rank = _get_rank(key, nodep->right, ope);
        return rank < 0 ? rank : (long)(nodep->left->size + nodep->count + rank);
    }
    else
    {
        return nodep->left->size;
    }
}

// from target node to root node, update the size
// src must not be RBTNIL
/// @brief update all nodes size when target node is deleted
/// @param self
/// @param src
void _update_size(BSTreeObject *self, RBNode *src)
{
    RBNode *nodep = src;
    while (nodep != RBTNIL)
    {
        nodep->size = nodep->count + nodep->left->size + nodep->right->size;
        nodep = nodep->parent;
    }
}

// get the node which has the same key as target from the root specified
// If not exist, get RBTNIL.
RBNode *_search(PyObject *key, RBNode *rootp, CompareOperator ope)
{
    RBNode *currentp = rootp;
    int comp_ret;
    while (currentp != RBTNIL && (comp_ret = _compare(key, currentp->key, ope)) != 0)
    {
        if (comp_ret == COMPARE_ERR)
        {
            PyErr_SetString(PyExc_TypeError, "Comparison Error");
            return NULL;
        }
        if (comp_ret > 0)
        {
            currentp = currentp->left;
        }
        else
        {
            currentp = currentp->right;
        }
    }
    return currentp;
}

// get the node which has the same key as target from the root
// If not exist, get the leaf node as a result of searching
RBNode *_search_fixup(PyObject *key, RBNode *rootp, CompareOperator ope)
{
    if (rootp == RBTNIL)
        return RBTNIL;
    RBNode *currentp = rootp;
    int comp_ret;
    while ((comp_ret = _compare(key, currentp->key, ope)) != 0)
    {
        if (comp_ret == COMPARE_ERR)
        {
            PyErr_SetString(PyExc_TypeError, "Comparison Error");
            return NULL;
        }
        if (comp_ret > 0 && currentp->left != RBTNIL)
            currentp = currentp->left;
        else if (comp_ret < 0 && currentp->right != RBTNIL)
            currentp = currentp->right;
        else
            break;
    }
    return currentp;
}

// key is an object which has > or < operator
RBNode *_create_rbnode(PyObject *obj, PyObject *keyfunc)
{
    RBNode *nodep = (RBNode *)malloc(sizeof(RBNode));
    if (nodep == NULL)
        return NULL;

    nodep->obj_list = NULL;
    if (_add_obj_to_rbnode(obj, nodep) == -1)
        return NULL;

    nodep->key = keyfunc == Py_None ? obj : PyObject_CallFunctionObjArgs(keyfunc, obj, NULL);
    if (nodep->key == NULL)
        return NULL;

    nodep->size = 1;
    nodep->count = 1;
    nodep->parent = RBTNIL;
    nodep->left = RBTNIL;
    nodep->right = RBTNIL;
    return nodep;
}

void _delete_rbnode(RBNode *nodep)
{
    // Py_DECREF(nodep->key);
    RBNode *current = nodep;
    while (current->obj_list != NULL)
    {
        ObjNode *next = current->obj_list->next;
        Py_DECREF(current->obj_list->obj);
        free(current->obj_list);
        current->obj_list = next;
    }
    free(nodep);
}

int _add_obj_to_rbnode(PyObject *new_obj, RBNode *node)
{
    ObjNode *objnodep = (ObjNode *)malloc(sizeof(ObjNode));
    if (!objnodep)
        return -1;
    Py_INCREF(new_obj);

    objnodep->obj = new_obj;
    objnodep->next = NULL;

    if (node->obj_list == NULL)
        node->obj_list = objnodep;
    else
    {
        ObjNode *current = node->obj_list;
        while (current->next != NULL)
            current = current->next;
        current->next = objnodep;
    }
    // [TODO] overflow check
    node->count++;
    return 0;
}

/// @brief delete the first object from the obj_list of the node
/// @param node
/// @return
int _delete_obj_from_rbnode(RBNode *node)
{
    if (node->obj_list == NULL)
        return -1;
    ObjNode *to_delete = node->obj_list;
    node->obj_list = node->obj_list->next;

    Py_DECREF(to_delete->obj);
    free(to_delete);
    node->count--;

    return 0;
}

// get the min node from the root.
// if rootp is RBTNIL, returns RBTNIL
RBNode *_get_min(RBNode *rootp)
{
    RBNode *currentp = rootp;
    while (currentp->left != RBTNIL)
        currentp = currentp->left;
    return currentp;
}

// get the max node from the root.
// if rootp is RBTNIL, returns RBTNIL
RBNode *_get_max(RBNode *rootp)
{
    RBNode *currentp = rootp;
    while (currentp->right != RBTNIL)
        currentp = currentp->right;
    return currentp;
}

/// @brief get the min object strictly larger than the arg key.
/// @param self
/// @param args pointer to python tuple
/// @return
static PyObject *
bstree_next(BSTreeObject *self, PyObject *args)
{
    // fetch the first arg
    if (!PyTuple_Check(args))
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    if (PyTuple_Size(args) != 1)
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    PyObject *obj = PyTuple_GetItem(args, 0);
    PyObject *key = self->keyfunc == Py_None ? obj : PyObject_CallFunctionObjArgs(self->keyfunc, obj, NULL);
    if (key == NULL)
        return NULL;

    RBNode *leafp = _search_fixup(key, self->root, self->ope);
    if (leafp == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    if (leafp == RBTNIL)
        Py_RETURN_NONE;
    int comp_ret = _compare(leafp->key, key, self->ope);
    if (comp_ret == COMPARE_ERR)
    {
        PyErr_SetString(PyExc_TypeError, "Comparison Error");
        return NULL;
    }
    else if (comp_ret < 0)
        return Py_BuildValue("O", leafp->key);
    else
    {
        RBNode *nextp = _get_next(leafp);
        if (nextp != RBTNIL)
            return Py_BuildValue("O", _get_next(leafp)->key);
        else
            Py_RETURN_NONE;
    }
}

/// @brief get the max object strictly smaller than the arg key.
/// @param self tree object
/// @param args pointer to python tuple
/// @return
static PyObject *
bstree_prev(BSTreeObject *self, PyObject *args)
{
    // fetch the first arg
    if (!PyTuple_Check(args))
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    if (PyTuple_Size(args) != 1)
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    PyObject *obj = PyTuple_GetItem(args, 0);
    PyObject *key = self->keyfunc == Py_None ? obj : PyObject_CallFunctionObjArgs(self->keyfunc, obj, NULL);
    if (key == NULL)
        return NULL;

    RBNode *leafp = _search_fixup(key, self->root, self->ope);
    if (leafp == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    if (leafp == RBTNIL)
        Py_RETURN_NONE;
    int comp_ret = _compare(leafp->key, key, self->ope);
    if (comp_ret == COMPARE_ERR)
    {
        PyErr_SetString(PyExc_TypeError, "Comparison Error");
        return NULL;
    }
    else if (comp_ret > 0)
        return Py_BuildValue("O", leafp->key);
    else
    {
        RBNode *prevp = _get_prev(leafp);
        if (prevp != RBTNIL)
            return Py_BuildValue("O", _get_prev(leafp)->key);
        else
            Py_RETURN_NONE;
    }
}

// get the node which is next to node
// if nothing, return RBTNIL
// assuming that node is in the tree
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

// get the node which is prev to node
// if nothing , return RBTNIL
// assuming that node is in the tree
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

void _left_rotate(BSTreeObject *self, RBNode *nodep)
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

void _right_rotate(BSTreeObject *self, RBNode *nodep)
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
// v could be RBTNIL
void _transplant(BSTreeObject *self, RBNode *nodeUp, RBNode *nodeVp)
{
    if (nodeUp->parent == RBTNIL)
        self->root = nodeVp;
    else if (nodeUp == nodeUp->parent->left)
        nodeUp->parent->left = nodeVp;
    else
        nodeUp->parent->right = nodeVp;
    // what happens when nodeVp is RBTNIL ?
    // can take arbitrary value
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
        // [TODO] implement "depth"
        {NULL}};

static PyMethodDef bstree_class_methods[] =
    {
        {"insert", (PyCFunction)bstree_insert, METH_VARARGS, "insert an object"},
        {"delete", (PyCFunction)bstree_delete, METH_VARARGS, "delete an object"},
        {"has", (PyCFunction)bstree_has, METH_VARARGS, "check if the object is in the tree"},
        {"to_list", (PyCFunction)bstree_list, METH_VARARGS | METH_KEYWORDS, "list object in order"},
        {"to_counter", (PyCFunction)bstree_counter, METH_NOARGS, "counter of objects"},
        {"next_to", (PyCFunction)bstree_next, METH_VARARGS, "get the next value"},
        {"prev_to", (PyCFunction)bstree_prev, METH_VARARGS, "get the prev value"},
        {"min", (PyCFunction)bstree_min, METH_NOARGS, "get the minimum value in the tree"},
        {"max", (PyCFunction)bstree_max, METH_NOARGS, "get the maximum value in the tree"},
        {"kth_smallest", (PyCFunction)bstree_kth_smallest, METH_VARARGS, "get the kth smallest value"},
        {"kth_largest", (PyCFunction)bstree_kth_largest, METH_VARARGS, "get the kth largest value"},
        {"rank", (PyCFunction)bstree_rank, METH_VARARGS, "get the rank of parameter"},
        {"clear", (PyCFunction)bstree_clear, METH_NOARGS, "clear the tree"},
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
static PyObject *bstree_modulefunc0(PyObject *module)
{
    return NULL;
}

// register module functions
static PyMethodDef bstree_module_methods[] =
    {
        {"func0", (PyCFunction)bstree_modulefunc0, METH_VARARGS, "doc for function in bstree module"},
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
