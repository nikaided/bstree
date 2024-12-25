#include "utils.h"

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
    self->ope = _lt;
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
    Py_RETURN_NONE;
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

    TypedKey *key = get_key_from(obj, self->keyfunc);
    if (key == NULL)
    {
        return NULL;
    }

    RBNode *yp = RBTNIL;
    RBNode *xp = self->root;
    while (xp != RBTNIL)
    {
        yp = xp;
        int comp_with_x;
        if ((comp_with_x = _compare(key, xp->key, self->ope)) == COMPARE_ERR)
            return NULL;

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
                ObjNode *objnodep = _create_objnode(obj);
                if (!objnodep)
                {
                    PyErr_SetString(PyExc_TypeError, "Create Objnode Error");
                    return NULL;
                }
                _add_objnode_to_rbnode(objnodep, xp);
                self->size += 1;
                _update_size(self, xp);
                Py_RETURN_TRUE;
            }
        }
    }

    ObjNode *objnodep2 = _create_objnode(obj);
    if (!objnodep2)
    {
        PyErr_SetString(PyExc_TypeError, "Create Objnode Error");
        return NULL;
    }
    // if the node doesn't exist, just increase the whole tree size.
    RBNode *nodep = _create_rbnode(key);
    if (nodep == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Create Node Error");
        return NULL;
    }
    _add_objnode_to_rbnode(objnodep2, nodep);
    self->size += 1;
    nodep->parent = yp;
    int comp_with_y;
    if (yp == RBTNIL)
        self->root = nodep;
    else if ((comp_with_y = _compare(key, yp->key, self->ope)) == COMPARE_ERR)
    {
        _delete_rbnode(nodep);
        return NULL;
    }
    else if (comp_with_y > 0)
        yp->left = nodep;
    else
        yp->right = nodep;
    _update_size(self, nodep);
    _insert_fixup(self, nodep);
    Py_RETURN_TRUE;
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
    int error;
    TypedKey *key = get_key_from(obj, self->keyfunc);
    if (key == NULL)
    {
        return NULL;
    }

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
    TypedKey *key = get_key_from(obj, self->keyfunc);
    if (key == NULL)
    {
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
    TypedKey *key = get_key_from(obj, self->keyfunc);
    if (key == NULL)
    {
        return NULL;
    }

    RBNode *leafp = _search_leaf(key, self->root, self->ope);
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
        return NULL;
    }
    else if (comp_ret < 0)
    {
        return Py_BuildValue("O", leafp->obj_list->obj);
    }
    else
    {
        RBNode *nextp = _get_next(leafp);
        if (nextp != RBTNIL)
        {
            return Py_BuildValue("O", _get_next(leafp)->obj_list->obj);
        }
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
    TypedKey *key = get_key_from(obj, self->keyfunc);
    if (key == NULL)
    {
        return NULL;
    }

    RBNode *leafp = _search_leaf(key, self->root, self->ope);
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
        return NULL;
    }
    else if (comp_ret > 0)
    {
        return Py_BuildValue("O", leafp->obj_list->obj);
    }
    else
    {
        RBNode *prevp = _get_prev(leafp);
        if (prevp != RBTNIL)
        {
            return Py_BuildValue("O", _get_prev(leafp)->obj_list->obj);
        }
        else
            Py_RETURN_NONE;
    }
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

/// almost equivalent to { sort(); bisect_left();}
// The key function in bstree is applied to the x while The key function in bisect_left is not applied to the x value.
// https://docs.python.org/3/library/bisect.html
//
static PyObject *
bstree_rank(BSTreeObject *self, PyObject *args)
{
    if (PyTuple_Size(args) != 1)
    {
        PyErr_SetString(PyExc_TypeError, "Argument Invalid");
        return NULL;
    }
    PyObject *obj = PyTuple_GetItem(args, 0);
    TypedKey *key = get_key_from(obj, self->keyfunc);
    if (key == NULL)
    {
        return NULL;
    }

    long rank = _get_rank(key, self->root, self->ope);
    if (rank < 0)
    {
        return NULL;
    }
    return Py_BuildValue("k", rank);
}

static PyMemberDef bstree_class_members[] =
    {
        {"size", T_LONG, offsetof(BSTreeObject, size), READONLY},
        // [TODO] implement "depth"
        {NULL}};

static PyMethodDef bstree_class_methods[] =
    {
        {"clear", (PyCFunction)bstree_clear, METH_NOARGS, "clear the tree"},
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
