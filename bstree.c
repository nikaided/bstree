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

typedef struct rbnode
{
    PyObject *key;
    unsigned long size;
    unsigned long count;
    char color;
    struct rbnode *parent;
    struct rbnode *left;
    struct rbnode *right;
} RBNode;

// compare a with b and return -1, 0, 1
typedef int (*CompareOperator)(const RBNode *, const RBNode *);

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
    PyObject *captured;
} BSTreeObject;

#endif // BSTREE_H

// private function declaration
RBNode *_create_node(PyObject *, char);
void _delete_node(RBNode *, char);
RBNode *_search(RBNode *, CompareOperator, PyObject *);
RBNode *_search_fixup(RBNode *, CompareOperator, PyObject *);
void _left_rotate(BSTreeObject *, RBNode *);
void _right_rotate(BSTreeObject *, RBNode *);
void _insert_fixup(BSTreeObject *, RBNode *);
void _update_size(BSTreeObject *, RBNode *);
void _delete_fixup(BSTreeObject *, RBNode *);
void _transplant(BSTreeObject *, RBNode *, RBNode *);
PyObject *_list_in_order(RBNode *, PyObject *, int *, char);
PyObject *_get_counter(RBNode *, PyObject *);
void _delete_all_nodes(RBNode *);

RBNode *_get_min(RBNode *);
RBNode *_get_max(RBNode *);
RBNode *_get_next(RBNode *);
RBNode *_get_prev(RBNode *);
unsigned long _get_rank(RBNode *, RBNode *, CompareOperator);
int _helper_smallest(RBNode *, unsigned long, PyObject **);
int _helper_largest(RBNode *, unsigned long, PyObject **);
void _increment_fixup(unsigned long *, enum IsDup);

int _lt_long(const RBNode *, const RBNode *);
int _lt_double(const RBNode *, const RBNode *);
int _lt_obj(const RBNode *, const RBNode *);
int _compare(const RBNode *, const RBNode *, CompareOperator);
int _can_be_treated_as_c_long(PyObject *);

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
static int bstree_init(BSTreeObject *self, PyObject *args, PyObject *kwargs)
{
    int dup = 0;
    static char *kwlists[] = {"dup", NULL};

    // dup argument is optional, and should be integer if provided.
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|i", kwlists, &dup))
        return -1;

    self->root = RBTNIL;
    self->size = 0;
    if (dup == 0)
        self->is_dup = NO_DUP;
    else
        self->is_dup = DUP;
    self->ope = NULL;
    return 0;
}

// clear the tree but keep the conf of dup
static PyObject *
bstree_clear(BSTreeObject *self, PyObject *args)
{
    if (self->root != RBTNIL)
        _delete_all_nodes(self->root);

    self->root = RBTNIL;
    self->size = 0;
    self->ope = NULL;
    Py_RETURN_NONE;
}

int _lt_long(const RBNode *a, const RBNode *b)
{
    long value_a = PyLong_AsLong(a->key);
    long value_b = PyLong_AsLong(b->key);

    if (value_a == -1 && PyErr_Occurred())
        return COMPARE_ERR;
    if (value_b == -1 && PyErr_Occurred())
        return COMPARE_ERR;

    return value_a < value_b ? 1 : 0;
}

int _lt_double(const RBNode *a, const RBNode *b)
{
    double value_a = PyFloat_AsDouble(a->key);
    double value_b = PyFloat_AsDouble(b->key);

    // a->key or b->key might be python int type
    if (value_a == -1 && PyErr_Occurred())
    {
        if ((value_a = (double)PyLong_AsLong(a->key)) != -1)
            PyErr_Clear();
        else
            return COMPARE_ERR;
    }
    if (value_b == -1 && PyErr_Occurred())
    {
        if ((value_b = (double)PyLong_AsLong(b->key)) != -1)
            PyErr_Clear();
        else
            return COMPARE_ERR;
    }
    return value_a < value_b ? 1 : 0;
}

// if a < b return 1, elif a >= b return 0, else return COMPARE_ERR.
int _lt_obj(const RBNode *a, const RBNode *b)
{
    PyObject *lt_name = PyUnicode_InternFromString("__lt__");
    PyObject *lt_result = PyObject_CallMethodObjArgs(a->key, lt_name, b->key, NULL);
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
    PyObject *gt_result = PyObject_CallMethodObjArgs(b->key, gt_name, a->key, NULL);
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
int _compare(const RBNode *a, const RBNode *b, CompareOperator comp)
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
int _can_be_treated_as_c_long(PyObject *obj)
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

    // validate the object and determine the comparison operator
    if (self->ope == NULL)
    {
        // python3 always treat any large or small integer as int type
        // but c can not handle it as long type if its value is too large or too small
        if (PyLong_Check(obj))
        {
            if (_can_be_treated_as_c_long(obj))
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
        else if (PyFloat_Check(obj))
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
        if (PyLong_Check(obj))
        {
            if (!_can_be_treated_as_c_long(obj))
            {
                self->ope = _lt_double;
                // printf("comp changed from _lt_long to _lt_double\n");
            }
        }
        else if (PyFloat_Check(obj))
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
        if (!PyFloat_Check(obj))
        {
            if (PyLong_Check(obj))
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

    // create a node first
    RBNode *nodep = _create_node(obj, INC_REF);

    RBNode *yp = RBTNIL;
    RBNode *xp = self->root;
    while (xp != RBTNIL)
    {
        yp = xp;
        int comp_with_x;
        if ((comp_with_x = _compare(nodep, xp, self->ope)) == COMPARE_ERR)
        {
            _delete_node(nodep, DEC_REF);
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
            _increment_fixup(&(xp->count), self->is_dup);
            _increment_fixup(&(self->size), self->is_dup);
            _update_size(self, xp);
            _delete_node(nodep, DEC_REF);
            Py_RETURN_NONE;
        }
    }
    // if the node doesn't exist, just increase the whole tree size.
    self->size += 1;
    nodep->parent = yp;
    int comp_with_y;
    if (yp == RBTNIL)
        self->root = nodep;
    else if ((comp_with_y = _compare(nodep, yp, self->ope)) == COMPARE_ERR)
    {
        _delete_node(nodep, DEC_REF);
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

// caution: obj is a pointer to python tuple
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

    if ((nodep = _search(self->root, self->ope, obj)) == RBTNIL)
    {
        PyErr_SetString(PyExc_ValueError, "Value not found in the tree");
        return NULL;
    }
    self->size -= 1;

    RBNode *yp = nodep;
    RBNode *xp, *wp;
    char y_original_color = yp->color;

    if (nodep->count > 1)
    {
        nodep->count -= 1;
        _update_size(self, nodep);
        Py_RETURN_NONE;
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
    _delete_node(nodep, DEC_REF);
    Py_RETURN_NONE;
}

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

    if (_search(self->root, self->ope, obj) == RBTNIL)
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
    if (PyTuple_Size(args) != 0)
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    PyObject *dict = PyDict_New();
    RBNode *node = self->root;
    if (node == RBTNIL)
        return dict;
    return _get_counter(node, dict);
}

static PyObject *
bstree_min(BSTreeObject *self, PyObject *args)
{
    if (PyTuple_Size(args) != 1)
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    RBNode *nodep = _get_min(self->root);
    if (nodep == RBTNIL)
        return NULL;
    return Py_BuildValue("O", nodep->key);
}

static PyObject *
bstree_max(BSTreeObject *self, PyObject *args)
{
    if (PyTuple_Size(args) != 1)
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    RBNode *nodep = _get_max(self->root);
    if (nodep == RBTNIL)
        return NULL;
    return Py_BuildValue("O", nodep->key);
}

static PyObject *
bstree_kth_smallest(BSTreeObject *self, PyObject *args)
{
    unsigned long k;
    int ret;
    PyObject *ans = NULL;
    if (!PyArg_ParseTuple(args, "|k", &k))
        return NULL;
    if (PyTuple_Size(args) == 0)
        k = 1;
    ret = _helper_smallest(self->root, k, &ans); // pointer to ans
    if (ret == -1)
        return NULL;
    return Py_BuildValue("O", ans);
}

static PyObject *
bstree_kth_largest(BSTreeObject *self, PyObject *args)
{
    unsigned long k;
    int ret;
    PyObject *ans = NULL;
    if (!PyArg_ParseTuple(args, "|k", &k))
        return NULL;
    if (PyTuple_Size(args) == 0)
        k = 1;
    ret = _helper_largest(self->root, k, &ans);
    if (ret == -1)
        return NULL;
    return Py_BuildValue("O", ans);
}

/// equivalent to (sort(); bisect_left();)
static PyObject *
bstree_rank(BSTreeObject *self, PyObject *args)
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
    RBNode *targetp = _create_node(obj, KEEP_REF);

    return Py_BuildValue("k", _get_rank(targetp, self->root, self->ope));
}

static PyObject *
_list_in_order(RBNode *node, PyObject *list, int *pidx, char is_reverse)
{
    if (is_reverse == 0)
    {
        if (node->left != RBTNIL)
            list = _list_in_order(node->left, list, pidx, is_reverse);

        for (int i = 0; i < node->count; i++)
            PyList_SET_ITEM(list, *pidx + i, Py_BuildValue("O", node->key));
        *pidx += node->count;

        if (node->right != RBTNIL)
            list = _list_in_order(node->right, list, pidx, is_reverse);
    }
    else
    {
        if (node->right != RBTNIL)
            list = _list_in_order(node->right, list, pidx, is_reverse);

        for (int i = 0; i < node->count; i++)
            PyList_SET_ITEM(list, *pidx + i, Py_BuildValue("O", node->key));
        *pidx += node->count;

        if (node->left != RBTNIL)
            list = _list_in_order(node->left, list, pidx, is_reverse);
    }
    return list;
}

void _delete_all_nodes(RBNode *node)
{
    if (node->left != RBTNIL)
        _delete_all_nodes(node->left);
    if (node->right != RBTNIL)
        _delete_all_nodes(node->right);
    _delete_node(node, DEC_REF);
}

PyObject *_get_counter(RBNode *node, PyObject *dict)
{
    if (node->left != RBTNIL)
        dict = _get_counter(node->left, dict);

    // check if node->key is hashable
    if (!PyObject_HasAttrString(node->key, "__hash__"))
    {
        // if not hashable, raise an error
        PyErr_SetString(PyExc_TypeError, "node->key is not hashable");
        return NULL;
    }
    PyDict_SetItem(dict, Py_BuildValue("O", node->key), Py_BuildValue("k", node->count));

    if (node->right != RBTNIL)
        dict = _get_counter(node->right, dict);

    return dict;
}

// [TODO] take care of overflow
void _increment_fixup(unsigned long *x, enum IsDup d)
{
    if (d == DUP)
        *x += 1;
}

int _helper_smallest(RBNode *node, unsigned long k, PyObject **ans)
{
    if (k > node->size)
    {
        PyErr_SetString(PyExc_IndexError, "Input index out of range");
        return -1;
    }
    if (node == RBTNIL)
    {
        return 0;
    }
    if (k <= node->left->size)
        return _helper_smallest(node->left, k, ans);
    else if (node->left->size < k && k <= node->left->size + node->count)
    {
        *ans = node->key; // update ans
        return 0;
    }
    else
        return _helper_smallest(node->right, k - node->left->size - node->count, ans);
}

int _helper_largest(RBNode *node, unsigned long k, PyObject **ans)
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

unsigned long _get_rank(RBNode *target, RBNode *node, CompareOperator ope)
{
    if (node == RBTNIL)
    {
        return 0;
    }
    int comp_with_x;
    if ((comp_with_x = _compare(target, node, ope)) == COMPARE_ERR)
    {
        PyErr_SetString(PyExc_TypeError, "Comparison Error");
        // [TODO] should not return NULL, what about 0?
        return 0;
    }
    if (comp_with_x > 0)
    {
        return _get_rank(target, node->left, ope);
    }
    else if (comp_with_x < 0)
    {
        return node->left->size + node->count + _get_rank(target, node->right, ope);
    }
    else
    {
        return node->left->size;
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

// get the node which key is obj, when nodep is a root
// If not exist, get RBTNIL.
RBNode *_search(RBNode *nodep, CompareOperator ope, PyObject *obj)
{
    RBNode *zp = nodep;
    RBNode *kp = _create_node(obj, KEEP_REF);
    int comp_ret;
    while (zp != RBTNIL && (comp_ret = _compare(kp, zp, ope)) != 0)
    {
        if (comp_ret == COMPARE_ERR)
        {
            PyErr_SetString(PyExc_TypeError, "Comparison Error");
            return NULL;
        }
        if (comp_ret > 0)
        {
            zp = zp->left;
        }
        else
        {
            zp = zp->right;
        }
    }
    return zp;
}

// get the node which key is k.
// If not exist, get RBTNIL.
RBNode *_search_fixup(RBNode *nodep, CompareOperator ope, PyObject *k)
{
    RBNode *zp = nodep;
    if (zp == RBTNIL)
        return RBTNIL;
    RBNode *kp = _create_node(k, KEEP_REF);
    int comp_ret;
    while ((comp_ret = _compare(kp, zp, ope)) != 0)
    {
        if (comp_ret == COMPARE_ERR)
        {
            PyErr_SetString(PyExc_TypeError, "Comparison Error");
            return NULL;
        }
        if (comp_ret > 0 && zp->left != RBTNIL)
            zp = zp->left;
        else if (comp_ret < 0 && zp->right != RBTNIL)
            zp = zp->right;
        else
            break;
    }
    return zp;
}

// key is an object which has > or < operator
RBNode *_create_node(PyObject *obj, char inc_ref)
{
    RBNode *nodep = malloc(sizeof(RBNode));
    if (nodep == NULL)
        return NULL;
    nodep->key = obj;
    nodep->size = 1;
    nodep->count = 1;
    nodep->parent = RBTNIL;
    nodep->left = RBTNIL;
    nodep->right = RBTNIL;
    if (inc_ref)
        Py_INCREF(obj);
    return nodep;
}

void _delete_node(RBNode *nodep, char dec_ref)
{
    if (dec_ref)
        Py_DECREF(nodep->key);
    free(nodep);
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

/// @brief get the key of the next node.
/// doesn't matter if the arg key is in the tree or not.
/// @param self
/// @param obj pointer to python tuple
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

    RBNode *nodep = _search_fixup(self->root, self->ope, obj);
    RBNode *kp = _create_node(obj, KEEP_REF);
    int comp_ret;
    if (nodep == RBTNIL)
        Py_RETURN_NONE;
    else if ((comp_ret = _compare(nodep, kp, self->ope)) == COMPARE_ERR)
    {
        PyErr_SetString(PyExc_TypeError, "Comparison Error");
        return NULL;
    }
    else if (comp_ret < 0)
        return Py_BuildValue("O", nodep->key);
    else
    {
        RBNode *nextp = _get_next(nodep);
        if (nextp != RBTNIL)
            return Py_BuildValue("O", _get_next(nodep)->key);
        else
            Py_RETURN_NONE;
    }
}

/// @brief get the key of the previous node.
/// doesn't matter if the arg key is in the tree or not.
/// @param self
/// @param obj pointer to python tuple
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

    RBNode *nodep = _search_fixup(self->root, self->ope, obj);
    RBNode *kp = _create_node(obj, KEEP_REF);
    int comp_ret;
    if (nodep == RBTNIL)
        Py_RETURN_NONE;
    else if ((comp_ret = _compare(nodep, kp, self->ope)) == COMPARE_ERR)
    {
        PyErr_SetString(PyExc_TypeError, "Comparison Error");
        return NULL;
    }
    else if (comp_ret > 0)
        return Py_BuildValue("O", nodep->key);
    else
    {
        RBNode *nextp = _get_prev(nodep);
        if (nextp != RBTNIL)
            return Py_BuildValue("O", _get_prev(nodep)->key);
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

// get the value of node which is prev to nodep
// if no node, return RBTNIL
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
        {NULL}};

static PyMethodDef bstree_class_methods[] =
    {
        {"insert", (PyCFunction)bstree_insert, METH_VARARGS, "insert an object"},
        {"delete", (PyCFunction)bstree_delete, METH_VARARGS, "delete an object"},
        {"has", (PyCFunction)bstree_has, METH_VARARGS, "check if the object is in the tree"},
        {"to_list", (PyCFunction)bstree_list, METH_VARARGS | METH_KEYWORDS, "list object in order"},
        {"to_counter", (PyCFunction)bstree_counter, METH_VARARGS, "counter of objects"},
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
