from datetime import datetime
from sys import getrefcount
import pytest

from bstree import BSTree
from tests.mock_object import NoCmpObj, LTObj, GTObj, LTGTObj

# class TestArgumentDataNumberError:
#     def test_when_insert_more_than_unsigned_long_times(self):
#         with pytest.raises(TypeError):
#             tree = BSTree()
#             for i in range(2**64):
#                 tree.insert(10)


class TestRefCount:
    def test_getrefcount(self):
        tree = BSTree()
        assert getrefcount(LTObj(10)) == 2
        # val = LTObj(10)
        # tree.insert(val)

        # assert getrefcount(val) == 2


class TestInputDataType:

    def test_when_insert_int(self):
        tree = BSTree()
        tree.insert(10)

    def test_when_insert_too_large_int(self):
        tree = BSTree()
        tree.insert(10**100)

    def test_when_insert_float(self):
        tree = BSTree()
        tree.insert(10.0)

    def test_when_insert_too_large_float(self):
        tree = BSTree()
        tree.insert(10.0**100)

    def test_when_insert_string(self):
        tree = BSTree()
        tree.insert("10")

    def test_when_insert_tuple(self):
        tree = BSTree()
        tree.insert((10))

    def test_when_insert_list(self):
        tree = BSTree()
        tree.insert([10])

    def test_when_insert_ltobj(self):
        tree = BSTree()
        tree.insert(LTObj(10))

    def test_when_insert_ltobj2(self):
        tree = BSTree()
        val = LTObj(10)
        tree.insert(val)

    def test_when_insert_gtobj(self):
        tree = BSTree()
        tree.insert(GTObj(10))

    def test_when_insert_datetime(self):
        tree = BSTree()
        tree.insert(datetime.now())


class TestInputDataTypeError:

    def test_when_insert_no_argument(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert()

    def test_when_insert_two_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert(0, 1)

    def test_when_insert_none(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert(None)


class TestInputDataTypes:

    def test_int_int(self):
        tree = BSTree()
        tree.insert(10)
        tree.insert(10**2)

    def test_int_largeint(self):
        tree = BSTree()
        tree.insert(10)
        tree.insert(10**100)

    def test_largeint_int(self):
        tree = BSTree()
        tree.insert(10**100)
        tree.insert(10)

    def test_largeint_largeint(self):
        tree = BSTree()
        tree.insert(10**100)
        tree.insert(10**1000)

    def test_int_float(self):
        tree = BSTree()
        tree.insert(10)
        tree.insert(10.0)

    def test_float_int(self):
        tree = BSTree()
        tree.insert(10.0)
        tree.insert(10)

    def test_float_float(self):
        tree = BSTree()
        tree.insert(10.0)
        tree.insert(10.0)

    def test_string_string(self):
        tree = BSTree()
        tree.insert("10")
        tree.insert("10 ** 2")

    def test_datetime_datetime(self):
        tree = BSTree()
        tree.insert(datetime.now())
        tree.insert(datetime.now())

    def test_int_float_int(self):
        tree = BSTree()
        tree.insert(10)
        tree.insert(10.0)
        tree.insert(10)

    def test_float_int_float(self):
        tree = BSTree()
        tree.insert(10.0)
        tree.insert(10)
        tree.insert(10.0)


class TestInputDataTypesError:

    def test_int_string(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert(10)
            tree.insert("10")

    def test_string_int(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert("10")
            tree.insert(10)

    def test_int_datetime(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert(10)
            tree.insert(datetime.now())

    def test_datetime_int(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert(datetime.now())
            tree.insert(10)


class TestInputDataTypesErrorUsingKey:

    def test_with_improper_key(self):
        key = lambda x: abs(x.val)
        with pytest.raises(AttributeError):
            tree = BSTree(key=key)
            tree.insert(0)


class TestInputDataTypesCustomObjs:

    def test_ltobj_ltobj(self):
        tree = BSTree()
        tree.insert(LTObj(10))
        tree.insert(LTObj(10**2))

    def test_ltobj_ltobj_2(self):
        tree = BSTree()
        val = LTObj(10)
        tree.insert(val)
        val = LTObj(10**2)
        tree.insert(val)

    def test_gtobj_gtobj(self):
        tree = BSTree()
        tree.insert(GTObj(10))
        tree.insert(GTObj(10**2))

    def test_ltobj_ltgtobj(self):
        tree = BSTree()
        tree.insert(LTObj(10))
        tree.insert(LTGTObj(10**2))

    def test_gtobj_ltgtobj_ng(self):
        tree = BSTree()
        tree.insert(GTObj(10))
        tree.insert(LTGTObj(10**2))

    def test_ltgtobj_ltobj(self):
        tree = BSTree()
        tree.insert(LTGTObj(10))
        tree.insert(LTObj(10**2))

    def test_ltgtobj_gtobj(self):
        tree = BSTree()
        tree.insert(LTGTObj(10))
        tree.insert(GTObj(10**2))

    def test_ltgtobj_ltgtobj(self):
        tree = BSTree()
        tree.insert(LTGTObj(10))
        tree.insert(LTGTObj(10**2))


class TestInputDataTypesCustomObjError:

    def test_nocmpobj_nocmpobj_error(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert(NoCmpObj(10))
            tree.insert(NoCmpObj(10**2))

    def test_nocmpobj_ltobj_error(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert(NoCmpObj(10))
            tree.insert(LTObj(10**2))

    def test_ltobj_nocmpobj_error(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert(LTObj(10))
            tree.insert(NoCmpObj(10**2))

    def test_ltobj_gtobj_error(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert(LTObj(10))
            tree.insert(GTObj(10**2))

    def test_gtobj_ltobj_error(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert(GTObj(10))
            tree.insert(LTObj(10**2))


class TestOutput:

    def test_when_ok(self):
        tree = BSTree()
        assert tree.insert(0) == True

    def test_when_ok_when_dup_is_true(self):
        tree = BSTree(dup=True)
        tree.insert(0)
        assert tree.insert(0) == True

    def test_when_ok_with_key(self):
        tree = BSTree(key=abs)
        assert tree.insert(0) == True

    def test_when_ng(self):
        tree = BSTree()
        tree.insert(0)
        assert tree.insert(0) == False

    def test_when_ng_with_key(self):
        tree = BSTree(key=abs)
        tree.insert(1)
        assert tree.insert(-1) == False
