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

    def test_next(self):
        tree = BSTree()
        tree.insert(0)
        tree.next_to(0)

    def test_prev(self):
        tree = BSTree()
        tree.insert(0)
        tree.prev_to(0)

class TestInputError:

    def test_next_with_no_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.next_to()

    def test_next_with_2_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.next_to(1,2)

    def test_prev_with_no_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.prev_to()

    def test_prev_with_2_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.prev_to(1,2)

    def test_next_with_invalid_compare(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert(0)
            tree.next_to("0")

    def test_prev_with_invalid_compare(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.insert(0)
            tree.prev_to("0")