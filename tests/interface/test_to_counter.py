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

    def test_type_error_when_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.to_counter(0)

class TestOutput:

    def test_when_tree_is_empty(self):
        tree = BSTree()
        assert tree.to_counter() == {}

    def test_int(self):
        for i in range(100):
            tree = BSTree(dup=True)
            n = randint(0, 100)
            insert_list = [randint(-pow(2, 7), pow(2, 7)) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = Counter(insert_list)
            actual = tree.to_counter()
            assert expected == actual

    def test_float(self):
        for i in range(100):
            tree = BSTree(dup=True)
            n = randint(0, 100)
            insert_list = [float(randint(-pow(2, 7), pow(2, 7))) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = Counter(insert_list)
            actual = tree.to_counter()
            assert expected == actual

    def test_string(self):
        for i in range(100):
            tree = BSTree(dup=True)
            n = randint(0, 100)
            insert_list = [str(randint(-pow(2, 7), pow(2, 7))) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = Counter(insert_list)
            actual = tree.to_counter()
            assert expected == actual

    def test_datetime(self):
        for i in range(100):
            tree = BSTree(dup=True)
            n = randint(0, 100)
            insert_list = [datetime.fromtimestamp(abs(randint(-pow(2, 7), pow(2, 7)))) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = Counter(insert_list)
            actual = tree.to_counter()
            assert expected == actual
    
    def test_LTObj(self):
        for i in range(100):
            tree = BSTree(dup=True)
            n = randint(0, 100)
            insert_list = [LTObj(randint(-pow(2, 7), pow(2, 7))) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = Counter(insert_list)
            actual = tree.to_counter()
            assert expected == actual


class TestOutputWithKey:

    # [TODO] the same as test_key in test_to_list.py
    def test_insert_different_objs_with_same_key(self):
        key = abs
        tree = BSTree(dup=True, key=key)
        insert_list = [-1, 1]
        for val in insert_list:
            tree.insert(val)
        expected = Counter([key(obj) for obj in insert_list])
        actual = tree.to_counter()
        assert expected == actual

    def test_insert_different_objs_with_same_key_then_delete(self):
        key = abs
        tree = BSTree(dup=True, key=key)
        insert_list = [-1, 1]
        for val in insert_list:
            tree.insert(val)
        delete = insert_list.pop()
        tree.delete(delete)
        expected = Counter([key(obj) for obj in insert_list])
        actual = tree.to_counter()
        assert expected == actual

    def test_insert_random_keys(self):
        for i in range(100):
            key = lambda x: abs(x.val)
            tree = BSTree(dup=True, key=key)
            n = randint(0, 100)
            insert_list = [LTObj(randint(-pow(2, 7), pow(2, 7))) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = Counter([key(obj) for obj in insert_list])
            actual = tree.to_counter()
            assert expected == actual
