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

    def test_min(self):
        tree = BSTree()
        tree.insert(0)
        tree.min()

    def test_max(self):
        tree = BSTree()
        tree.insert(0)
        tree.max()

    def test_kth_smallest_with_argument(self):
        tree = BSTree()
        tree.insert(0)
        tree.kth_smallest(1)

    def test_kth_largest_with_argument(self):
        tree = BSTree()
        tree.insert(0)
        tree.kth_largest(1)


class TestInputError:

    def test_min_with_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.min(1)

    def test_max_with_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.max(1)

    def test_kth_smallest_with_zero(self):
        with pytest.raises(ValueError):
            tree = BSTree()
            tree.kth_smallest(0)

    def test_kth_smallest_with_negative_k(self):
        with pytest.raises(ValueError):
            tree = BSTree()
            tree.kth_smallest(-1)

    def test_kth_smallest_with_exceed_size_k(self):
        with pytest.raises(ValueError):
            tree = BSTree()
            tree.kth_smallest(1)

    def test_kth_largest_with_zero(self):
        with pytest.raises(ValueError):
            tree = BSTree()
            tree.kth_largest(0)

    def test_kth_largest_with_negative_k(self):
        with pytest.raises(ValueError):
            tree = BSTree()
            tree.kth_largest(-1)

    def test_kth_smallest_with_exceed_size_k(self):
        with pytest.raises(ValueError):
            tree = BSTree()
            tree.kth_largest(1)


class TestOutput:

    def test_min(self):
        tree = BSTree()
        tree.insert(1)
        tree.insert(2)
        assert tree.min() == 1
    
    def test_max(self):
        tree = BSTree()
        tree.insert(1)
        tree.insert(2)
        assert tree.max() == 2

    def test_kth_smallest(self):
        tree = BSTree()
        tree.insert(1)
        tree.insert(2)
        assert tree.kth_smallest(2) == 2

    def test_kth_largest(self):
        tree = BSTree()
        tree.insert(1)
        tree.insert(2)
        assert tree.kth_largest(2) == 1

class TestOutputError:

    def test_min_with_empty_tree(self):
        with pytest.raises(ValueError):
            tree = BSTree()
            assert tree.min()

    def test_max_with_empty_tree(self):
        with pytest.raises(ValueError):
            tree = BSTree()
            assert tree.max() is None

    def test_kth_smallest_with_empty_tree(self):
        with pytest.raises(ValueError):
            tree = BSTree()
            tree.kth_smallest()

    def test_kth_largest_with_empty_tree(self):
        with pytest.raises(ValueError):
            tree = BSTree()
            tree.kth_largest()

class TestOutputWithKey:

    def test_min(self):
        tree = BSTree(key=abs)
        tree.insert(1)
        tree.insert(-1)
        assert tree.min() == -1
    
    def test_max(self):
        tree = BSTree(key=abs)
        tree.insert(1)
        tree.insert(-1)
        assert tree.max() == 1

    def test_min_LTObj(self):
        tree = BSTree(key=lambda x: abs(x.val))
        tree.insert(LTObj(1))
        tree.insert(LTObj(0))
        assert tree.min() == LTObj(0)

    def test_max_LTObj(self):
        tree = BSTree(key=lambda x: abs(x.val))
        tree.insert(LTObj(1))
        tree.insert(LTObj(0))
        assert tree.max() == LTObj(1)
