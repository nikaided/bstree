import sys
from random import randint
import pytest
from bstree import BSTree


class TestRBTree:
    def test_insert_random_char(self):
        tree = BSTree()
        for i in range(10**6):
            tree.insert(randint(-pow(2, 7), pow(2, 7)))
        val = -pow(2, 7)
        for v in tree.to_list():
            assert val <= v
            val = v

    def test_insert_random_short(self):
        tree = BSTree()
        for i in range(10**6):
            tree.insert(randint(-pow(2, 15), pow(2, 15) - 1))
        val = -pow(2, 15)
        for v in tree.to_list():
            assert val <= v
            val = v

    def test_insert_random_long(self):
        tree = BSTree()
        for i in range(10**6):
            tree.insert(randint(-pow(2, 31), pow(2, 31) - 1))
        val = -pow(2, 31)
        for v in tree.to_list():
            assert val <= v
            val = v

    def test_insert_max_size(self):
        tree = BSTree()
        tree.insert(sys.maxsize)
        val = 0
        for v in tree.to_list():
            assert val <= v
            val = v
