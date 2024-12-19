from datetime import datetime
import sys
from bisect import bisect_left
from collections import Counter
from random import randint, choice, shuffle, sample

import pytest

from bstree import BSTree
from tests.mock_object import LTObj


class TestInit:

    def test_init_class(self):
        tree = BSTree()
        assert isinstance(tree, BSTree)

    def test_init_size(self):
        tree = BSTree()
        assert tree.size == 0

    def test_init_list(self):
        tree = BSTree()
        assert tree.to_list() == []

    def test_init_counter(self):
        tree = BSTree()
        assert tree.to_counter() == {}

    def test_dup_argument(self):
        tree = BSTree(True)
        tree = BSTree(False)
        assert True

    def test_dup_argument_as_kwarg(self):
        tree = BSTree(dup=True)
        tree = BSTree(dup=False)
        assert True

    def test_key_argument(self):
        tree = BSTree(True, abs)
        tree = BSTree(False, lambda x: len(x))

        def myfunc(x):
            return x**2 + 2 * x + 1

        tree = BSTree(True, myfunc)
        assert True

    def test_key_argument_as_kwarg(self):
        tree = BSTree(key=abs)
        tree = BSTree(key=lambda x: len(x))

        def myfunc(x):
            return x**2 + 2 * x + 1

        tree = BSTree(key=myfunc)

        class MyClass:
            def __init__(self, x):
                self.x = x

            def mylen(self):
                return len(self.x)

        tree = BSTree(key=MyClass.mylen)
        assert True

    def test_tricky_dup_argument(self):
        tree = BSTree(0 == 0)
        tree = BSTree(0 == 1)
        tree = BSTree(dup=1 == 1)
        tree = BSTree(dup=0 == 1)
        assert True


class TestInitError:

    def test_error_if_no_dup_and_key(self):
        with pytest.raises(TypeError):
            tree = BSTree(len)

    def test_error_if_dup_is_int(self):
        with pytest.raises(TypeError):
            tree = BSTree(1)

    def test_error_if_dup_is_float(self):
        """want to raise exception but not satisfied yet
        because python evauates non-zero value as True"""
        with pytest.raises(TypeError):
            tree = BSTree(0.1)

    def test_error_if_key_is_not_callable(self):
        with pytest.raises(TypeError):
            tree = BSTree(key=0)
