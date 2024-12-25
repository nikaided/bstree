from datetime import datetime
from sys import getrefcount
import pytest

from bstree import BSTree
from tests.mock_object import NoCmpObj, LTObj, GTObj, LTGTObj


class TestInputDataType:

    def test_when_delete_int_after_insert_int(self):
        tree = BSTree()
        tree.insert(10)
        tree.delete(10)

    def test_when_delete_int_after_insert_int_when_dup_is_true(self):
        tree = BSTree(dup=True)
        tree.insert(0)
        tree.delete(0)

    def test_when_delete_float_after_insert_float(self):
        tree = BSTree()
        tree.insert(10.0)
        tree.delete(10.0)

    def test_when_delete_int_after_insert_float(self):
        tree = BSTree()
        tree.insert(10.0)
        tree.delete(10)

    def test_when_delete_float_after_insert_int(self):
        tree = BSTree()
        tree.insert(10)
        tree.delete(10.0)

    def test_when_delete_string_after_insert_string(self):
        tree = BSTree()
        tree.insert("10")
        tree.delete("10")

    def test_when_delete_tuple_after_insert_tuple(self):
        tree = BSTree()
        tree.insert((10, 20))
        tree.delete((10, 20))

    def test_when_delete_list_after_insert_list(self):
        tree = BSTree()
        tree.insert([10, 20])
        tree.delete([10, 20])

    def test_when_delete_ltobj_after_insert_ltobj(self):
        tree = BSTree()
        tree.insert(LTObj(10))
        tree.delete(LTObj(10))

    def test_when_insert_ltobj2(self):
        tree = BSTree()
        val = LTObj(10)
        tree.insert(val)

    def test_when_delete_ltobj_after_insert_ltobj_2(self):
        tree = BSTree()
        val = LTObj(10)
        tree.insert(val)
        tree.delete(val)

    def test_when_delete_ltobj2_after_insert_ltobj_2(self):
        tree = BSTree()
        val1 = LTObj(10)
        val2 = LTObj(10)
        tree.insert(val1)
        tree.delete(val2)

    def test_when_delete_gtobj_after_insert_gtobj(self):
        tree = BSTree()
        tree.insert(GTObj(10))
        tree.delete(GTObj(10))

    def test_when_delete_datetime_after_insert_datetime(self):
        tree = BSTree()
        val = datetime.now()
        tree.insert(val)
        tree.delete(val)


class TestInputDataTypeError:

    def test_when_no_inputs(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.delete()

    def test_when_two_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.delete(0, 1)

    def test_when_delete_what_is_not_inserted(self):
        with pytest.raises(ValueError):
            tree = BSTree()
            tree.delete(0)


class TestInputDataTypeWithKey:

    def test_delete_int_after_insert_int(self):
        tree = BSTree(key=abs)
        tree.insert(0)
        tree.delete(0)

    def test_delete_int_after_insert_int_when_dup_is_true(self):
        tree = BSTree(dup=True, key=abs)
        tree.insert(0)
        tree.delete(0)

    # [TODO] This is the typical case that the objects are totally different while the keys are the same 
    def test_delete_int_after_insert_int(self):
        tree = BSTree(key=abs)
        tree.insert(1)
        tree.delete(-1)


class TestInputDataTypeErrorWithKey:

    def test_type_error_when_no_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree(key=abs)
            tree.delete()

    def test_type_error_when_two_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree(key=abs)
            tree.delete(0, 1)

    def test_when_delete_what_is_not_inserted(self):
        with pytest.raises(ValueError):
            tree = BSTree(key=abs)
            tree.delete(0)

