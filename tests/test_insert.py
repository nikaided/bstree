from datetime import datetime
import pytest

from bstree import BSTree
from mock_object import NoCmpObj, LTObj, GTObj, LTGTObj


class TestArgumentDataType:
    def test_type_error_when_no_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert()

    def test_type_error_when_two_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert(0, 1)

    def test_when_insert_int(self):
        tree = BSTree()
        tree.insert(10)
        assert True

    def test_when_insert_too_large_int(self):
        tree = BSTree()
        tree.insert(10**100)
        assert True

    def test_when_insert_float(self):
        tree = BSTree()
        tree.insert(10)
        assert True

    def test_when_insert_too_large_float(self):
        tree = BSTree()
        tree.insert(10.0**100)
        assert True

    def test_when_insert_string(self):
        tree = BSTree()
        tree.insert("10")
        assert True

    def test_when_insert_tuple(self):
        tree = BSTree()
        tree.insert((10))
        assert True

    def test_when_insert_list(self):
        tree = BSTree()
        tree.insert([10])
        assert True

    def test_when_insert_custom_ltobj(self):
        tree = BSTree()
        tree.insert(LTObj(10))
        assert True

    def test_when_insert_custom_gtobj(self):
        tree = BSTree()
        tree.insert(GTObj(10))
        assert True

    def test_when_insert_datetime(self):
        tree = BSTree()
        tree.insert(datetime.now())
        assert True

    def test_when_insert_none(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert(None)


class TestCompareDataTypes:
    def test_int_int(self):
        tree = BSTree()
        tree.insert(10)
        tree.insert(10**2)
        tree.to_list()
        assert True

    def test_int_largeint(self):
        tree = BSTree()
        tree.insert(10)
        tree.insert(10**100)
        print(tree.to_list())
        assert True

    def test_largeint_int(self):
        tree = BSTree()
        tree.insert(10**100)
        tree.insert(10)
        print(tree.to_list())
        assert True

    # [TODO] investigate
    def test_largeint_largeint(self):
        tree = BSTree()
        tree.insert(10**100)
        tree.insert(10**1000)
        print(tree.to_list())
        assert True

    def test_int_float(self):
        tree = BSTree()
        tree.insert(10)
        tree.insert(10.0)
        print(tree.to_list())
        assert True

    def test_float_int(self):
        tree = BSTree()
        tree.insert(10.0)
        tree.insert(10)
        print(tree.to_list())
        assert True

    def test_float_float(self):
        tree = BSTree()
        tree.insert(10.0)
        tree.insert(10.0)
        print(tree.to_list())
        assert True

    def test_string_string(self):
        tree = BSTree()
        tree.insert("10")
        tree.insert("10 ** 2")
        print(tree.to_list())
        assert True

    def test_datetime_datetime(self):
        tree = BSTree()
        tree.insert(datetime.now())
        tree.insert(datetime.now())
        print(tree.to_list())
        assert True


class TestCompareDataTypesError:
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


class TestCustomObjArguments:
    def test_ltobj_ltobj(self):
        tree = BSTree()
        tree.insert(LTObj(10))
        tree.insert(LTObj(10**2))
        print([obj.val for obj in tree.to_list()])
        assert True

    def test_gtobj_gtobj(self):
        tree = BSTree()
        tree.insert(GTObj(10))
        tree.insert(GTObj(10**2))
        print([obj.val for obj in tree.to_list()])
        assert True

    def test_ltobj_ltgtobj(self):
        tree = BSTree()
        tree.insert(LTObj(10))
        tree.insert(LTGTObj(10**2))
        print([obj.val for obj in tree.to_list()])
        assert True

    def test_gtobj_ltgtobj_ng(self):
        tree = BSTree()
        tree.insert(GTObj(10))
        tree.insert(LTGTObj(10**2))
        print([obj.val for obj in tree.to_list()])
        assert True

    def test_ltgtobj_ltobj(self):
        tree = BSTree()
        tree.insert(LTGTObj(10))
        tree.insert(LTObj(10**2))
        print([obj.val for obj in tree.to_list()])
        assert True

    def test_ltgtobj_gtobj(self):
        tree = BSTree()
        tree.insert(LTGTObj(10))
        tree.insert(GTObj(10**2))
        print([obj.val for obj in tree.to_list()])
        assert True

    def test_ltgtobj_ltgtobj(self):
        tree = BSTree()
        tree.insert(LTGTObj(10))
        tree.insert(LTGTObj(10**2))
        print([obj.val for obj in tree.to_list()])
        assert True


class TestCustomObjArgumentError:
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
