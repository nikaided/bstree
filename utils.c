#include "utils.h"

// every leaf is treated as the same node
// left, right, parent can take an arbitrary value
RBNode sentinel =
    {
        .color = BLACK,
        .left = RBTNIL,
        .right = RBTNIL,
        .parent = NULL,
        .size = 0};

PyObject *_list_in_order(RBNode *node, PyObject *list, int *pidx, char is_reverse)
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

int _add_counter(RBNode *node, PyObject *dict)
{
    if (node->left != RBTNIL && _add_counter(node->left, dict) == -1)
        return -1;

    // check if node->key is hashable
    // if (node->key->type ==  KEY_LONG && !PyObject_HasAttrString(node->key->value.lval, "__hash__"))
    //     return -1;
    // if (node->key->type ==  KEY_DOUBLE && !PyObject_HasAttrString(node->key->value.dval, "__hash__"))
    //     return -1;
    // if (node->key->type ==  KEY_OBJECT && !PyObject_HasAttrString(node->key->value.obj, "__hash__"))
    //     return -1;

    if (node->key->type == KEY_LONG)
    {
        if (PyDict_SetItem(dict, Py_BuildValue("l", node->key->value.lval), Py_BuildValue("k", node->count)) == -1)
            return -1;
    }
    else if (node->key->type == KEY_DOUBLE)
    {
        if (PyDict_SetItem(dict, Py_BuildValue("d", node->key->value.dval), Py_BuildValue("k", node->count)) == -1)
            return -1;
    }
    else
    {
        if (PyDict_SetItem(dict, Py_BuildValue("O", node->key->value.obj), Py_BuildValue("k", node->count)) == -1)
            return -1;
    }

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

long _get_rank(TypedKey *key, RBNode *nodep, CompareOperator ope)
{
    if (nodep == RBTNIL)
    {
        return 0;
    }
    int comp_with_x = _compare(key, nodep->key, ope);
    if (comp_with_x == COMPARE_ERR)
    {
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
RBNode *_search(TypedKey *key, RBNode *rootp, CompareOperator ope)
{
    RBNode *currentp = rootp;
    int comp_ret;
    while (currentp != RBTNIL && (comp_ret = _compare(key, currentp->key, ope)) != 0)
    {
        if (comp_ret == COMPARE_ERR)
        {
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
RBNode *_search_leaf(TypedKey *key, RBNode *rootp, CompareOperator ope)
{
    if (rootp == RBTNIL)
        return RBTNIL;
    RBNode *currentp = rootp;
    int comp_ret;
    while ((comp_ret = _compare(key, currentp->key, ope)) != 0)
    {
        if (comp_ret == COMPARE_ERR)
        {
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
RBNode *_create_rbnode(TypedKey *key)
{
    RBNode *nodep = (RBNode *)malloc(sizeof(RBNode));
    if (nodep == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    nodep->key = key;
    nodep->obj_list = NULL;

    nodep->count = 0;
    nodep->color = RED;
    nodep->size = 1;
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
    free(nodep->key);
    free(nodep);
}

ObjNode *_create_objnode(PyObject *obj)
{
    ObjNode *objnodep = (ObjNode *)malloc(sizeof(ObjNode));
    if (!objnodep)
    {
        PyErr_NoMemory();
        return NULL;
    }
    Py_INCREF(obj);

    objnodep->obj = obj;
    objnodep->next = NULL;
    return objnodep;
}

int _add_objnode_to_rbnode(ObjNode *objnodep, RBNode *node)
{
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

void _delete_all_rbnodes(RBNode *node)
{
    if (node->left != RBTNIL)
        _delete_all_rbnodes(node->left);
    if (node->right != RBTNIL)
        _delete_all_rbnodes(node->right);
    _delete_rbnode(node);
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

// if a < b return 1, elif a > b return -1, elif a == b return 0 else return COMPARE_ERR
int _compare(TypedKey *a, TypedKey *b, CompareOperator comp)
{
    int a_comp_b = comp(a, b);
    if (a_comp_b == COMPARE_ERR)
    {
        return COMPARE_ERR;
    }
    int b_comp_a = comp(b, a);
    if (b_comp_a == COMPARE_ERR)
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
        PyErr_SetString(PyExc_TypeError, "Inconsistent comparison results between comp(a, b) and comp(b, a)");
        return COMPARE_ERR;
    }
}

// if a < b return 1, elif a >= b return 0, else return COMPARE_ERR.
int _lt_obj(PyObject *a, PyObject *b)
{
    // call rich compare slot
    // if a < b
    PyObject *lt = a->ob_type->tp_richcompare(a, b, Py_LT);
    if (!lt)
    {
        PyErr_SetString(PyExc_TypeError, "Failed to evaluate a.__lt__(b)");
        return COMPARE_ERR;
    }

    if (lt == Py_True)
        return 1;
    else if (lt == Py_False)
        return 0;
    Py_DECREF(lt);

    // if b > a
    PyObject *gt = b->ob_type->tp_richcompare(b, a, Py_GT);
    if (!gt)
    {
        PyErr_SetString(PyExc_TypeError, "Failed to evaluate b.__gt__(a)");
        return COMPARE_ERR;
    }

    if (gt == Py_True)
        return 1;
    else if (gt == Py_False)
        return 0;
    Py_DECREF(gt);
    PyErr_SetString(PyExc_TypeError, "Failed to compare a with b");
    return COMPARE_ERR;
}

int _lt(TypedKey *a, TypedKey *b)
{
    if (a->type == KEY_LONG)
    {
        if (b->type == KEY_LONG)
            return a->value.lval < b->value.lval ? 1 : 0;
        if (b->type == KEY_DOUBLE)
            return a->value.lval < b->value.dval ? 1 : 0;
        return _lt_obj(Py_BuildValue("l", a->value.lval), b->value.obj);
    }
    if (a->type == KEY_DOUBLE)
    {
        if (b->type == KEY_DOUBLE)
            return a->value.dval < b->value.dval ? 1 : 0;
        if (b->type == KEY_LONG)
            return a->value.dval < b->value.lval ? 1 : 0;
        return _lt_obj(Py_BuildValue("d", a->value.dval), b->value.obj);
    }
    if (a->type == KEY_OBJECT)
    {
        if (b->type == KEY_LONG)
            return _lt_obj(a->value.obj, Py_BuildValue("l", b->value.lval));
        if (b->type == KEY_DOUBLE)
            return _lt_obj(a->value.obj, Py_BuildValue("d", b->value.dval));
        return _lt_obj(a->value.obj, b->value.obj);
    }
}

TypedKey *get_key_from(PyObject *obj, PyObject *keyfunc)
{
    TypedKey *ret = (TypedKey *)malloc(sizeof(TypedKey));
    if (ret == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }
    PyObject *keyObj = keyfunc == Py_None ? obj : PyObject_CallFunctionObjArgs(keyfunc, obj, NULL);
    if (keyObj == NULL)
    {
        free(ret);
        return NULL;
    }
    // python3 always treat any large or small integer as int type
    // but c can not handle it as long type if its value is too large or too small
    if (PyLong_Check(keyObj))
    {
        if (get_long_from(keyObj, &(ret->value.lval)) != -1)
        {
            ret->type = KEY_LONG;
        }
        else if (get_double_from(keyObj, &(ret->value.dval)) != -1)
        {
            ret->type = KEY_DOUBLE;
        }
        else
        {
            ret->type = KEY_OBJECT;
            ret->value.obj = obj;
        }
    }
    else if (PyFloat_Check(keyObj))
    {
        if (get_double_from(keyObj, &(ret->value.dval)) != -1)
        {
            ret->type = KEY_DOUBLE;
        }
        else
        {
            ret->type = KEY_OBJECT;
            ret->value.obj = obj;
        }
    }
    else
    {
        ret->type = KEY_OBJECT;
        ret->value.obj = obj;
    }
    return ret;
}

int get_long_from(PyObject *obj, long *ans)
{
    if ((*ans = PyLong_AsLong(obj)) == -1 && PyErr_Occurred())
    {
        PyErr_Clear();
        return -1;
    }
    return 0;
}

int get_double_from(PyObject *obj, double *ans)
{
    if ((*ans = PyFloat_AsDouble(obj)) == -1 && PyErr_Occurred())
    {
        PyErr_Clear();
        return -1;
    }
    return 0;
}
