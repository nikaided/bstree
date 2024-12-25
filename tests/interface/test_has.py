from datetime import datetime
from sys import getrefcount
import pytest

from bstree import BSTree
from tests.mock_object import NoCmpObj, LTObj, GTObj, LTGTObj


class TestInput:

    def test_int_when_double_is_inserted(self):
        tree = BSTree()
        tree.insert(0.0)
        assert tree.has(0)

    def test_float_when_int_is_inserted(self):
        tree = BSTree()
        tree.insert(0)
        assert tree.has(0.0)


class TestInputError:
    def test_when_no_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.has()

    def test_when_two_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.has(0, 1)
