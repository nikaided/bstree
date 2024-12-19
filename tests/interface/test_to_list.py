# This module is mainly responsible for the input and output interface
 
from collections import Counter
from datetime import datetime
import sys
from bisect import bisect_left
from collections import Counter
from random import randint, choice, shuffle, sample

import pytest

from bstree import BSTree
from tests.mock_object import LTObj


class TestInput:

    def test_kwarg_true(self):
        tree = BSTree()
        tree.to_list(reverse=True)

    def test_kwarg_false(self):
        tree = BSTree()
        tree.to_list(reverse=False)

    def test_arg_tricky(self):
        tree = BSTree()
        with pytest.raises(TypeError):
            tree.to_list(1)

    def test_kwarg_tricky(self):
        tree = BSTree()
        with pytest.raises(TypeError):
            tree.to_list(reverse=1)


class TestOutput:

    def test_when_tree_is_empty(self):
        tree = BSTree()
        assert tree.to_list() == []

    def test_int(self):
        for i in range(100):
            tree = BSTree()
            n = randint(0, 100)
            insert_list = [randint(-pow(2, 7), pow(2, 7)) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = list(set(insert_list))
            expected.sort()
            actual = tree.to_list(reverse=False)
            assert expected == actual

    def test_float(self):
        for i in range(100):
            tree = BSTree()
            n = randint(0, 100)
            insert_list = [float(randint(-pow(2, 7), pow(2, 7))) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = list(set(insert_list))
            expected.sort()
            actual = tree.to_list(reverse=False)
            assert expected == actual
    
    def test_string(self):
        for i in range(100):
            tree = BSTree()
            n = randint(0, 100)
            insert_list = [str(randint(-pow(2, 7), pow(2, 7))) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = list(set(insert_list))
            expected.sort()
            actual = tree.to_list(reverse=False)
            assert expected == actual

    def test_LTObj(self):
        for i in range(100):
            tree = BSTree()
            n = randint(0, 100)
            insert_list = [LTObj(randint(-pow(2, 7), pow(2, 7))) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = list(set(insert_list))
            expected.sort()
            actual = tree.to_list(reverse=False)
            assert expected == actual
    
    def test_datetime(self):
        for i in range(100):
            tree = BSTree()
            n = randint(0, 100)
            insert_list = [datetime.fromtimestamp(abs(randint(-pow(2, 7), pow(2, 7)))) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = list(set(insert_list))
            expected.sort()
            actual = tree.to_list(reverse=False)
            assert expected == actual


class TestOutputWithKey:

    # [TODO] OK to fail
    def test_insert_different_objs_with_same_key(self):
        key = abs
        tree = BSTree(dup=True, key=key)
        tree.insert(-1)
        tree.insert(1)
        assert tree.to_list() == [-1, 1]

    # [TODO] OK to fail
    def test_insert_different_objs_with_same_key_then_delete(self):
        key = abs
        tree = BSTree(dup=True, key=key)
        tree.insert(-1)
        tree.insert(1)
        tree.delete(-1)
        assert tree.to_list() == [1]

    # [TODO] OK to fail
    def test_insert_different_objs_with_same_key_then_delete_2(self):
        key = abs
        tree = BSTree(dup=True, key=key)
        tree.insert(-1)
        tree.insert(1)
        tree.delete(1)
        assert tree.to_list() == [-1]

    def test_insert_random_keys(self):    
        for i in range(100):
            key = lambda x: abs(x.val)
            tree = BSTree(dup=True, key=key)
            n = randint(0, 100)
            insert_list = [LTObj(randint(-pow(2, 7), pow(2, 7))) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = insert_list
            expected.sort(key=key)
            expected = [key(obj) for obj in expected]
            actual = [key(obj) for obj in tree.to_list()]
            assert expected == actual
