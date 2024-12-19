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

    def test_when_empty_tree(self):
        tree = BSTree()
        tree.rank(0)

    def test_int_arg_when_tree_is_int(self):
        tree = BSTree()
        tree.insert(0)
        tree.rank(0)

    def test_float_arg_when_tree_is_float(self):
        tree = BSTree()
        tree.insert(0.0)
        tree.rank(0.0)


class TestInputError:

    def test_when_no_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.rank()

    def test_when_two_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.rank(0, 1)


class TestOutput:

    def test_when_empty_tree(self):
        tree = BSTree()
        assert tree.rank(0) == 0

    def test_int_arg_when_tree_is_int(self):
        tree = BSTree()
        tree.insert(0)
        assert tree.rank(0) == 0

    def test_float_arg_when_tree_is_float(self):
        tree = BSTree()
        tree.insert(0.0)
        assert tree.rank(0.0) == 0
        
