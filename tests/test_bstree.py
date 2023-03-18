import sys
from random import randint
import pytest
from bstree import BSTree


class TestRBTreeInit:
    def test_init_size(self):
        tree = BSTree()
        assert tree.size == 0

    def test_init_list(self):
        tree = BSTree()
        assert tree.to_list() == []


class TestRBTreeInsert:
    def test_insert_float(self):
        tree = BSTree()
        with pytest.raises(TypeError):
            tree.insert(1.0)

    def test_insert_string(self):
        tree = BSTree()
        with pytest.raises(TypeError):
            tree.insert("0")

    def test_size_when_inserted(self):
        for i in range(100):
            tree = BSTree()
            n = randint(0, 100)
            for j in range(n):
                tree.insert(randint(-pow(2, 7), pow(2, 7) - 1))
            assert tree.size == n

    def test_size_when_inserted_same_value(self):
        tree = BSTree()
        n = randint(0, 100)
        for j in range(n):
            tree.insert(0)
        assert tree.size == n

    def test_insert_random_char(self):
        for i in range(10**3):
            tree = BSTree()
            for j in range(10**3):
                tree.insert(randint(-pow(2, 7), pow(2, 7) - 1))
            val = -pow(2, 7)
            for v in tree.to_list():
                assert val <= v
                val = v

    def test_insert_system_max_value(self):
        tree = BSTree()
        tree.insert(sys.maxsize)
        val = 0
        for v in tree.to_list():
            assert val <= v
            val = v


class TestRBTreeSearch:
    def test_search_inserted(self):
        tree = BSTree()
        tree.insert(0)
        assert tree.search(0)

    def test_search_not_inserted(self):
        tree = BSTree()
        tree.insert(0)
        assert not tree.search(1)


class TestRBTreeDelete:
    def test_size_when_deleted(self):
        for i in range(100):
            tree = BSTree()
            m = randint(100, 200)
            for j in range(m):
                tree.insert(j)
            n = randint(0, 100)
            for j in range(n):
                tree.delete(j)
            assert tree.size == m - n

    def test_delete_not_inserted(self):
        with pytest.raises(SystemError):
            tree = BSTree()
            tree.delete(0)

    def test_if_delete_times_inserted(self):
        tree = BSTree()
        for i in range(10):
            tree.insert(0)
        for i in range(10):
            tree.delete(0)
        assert tree.size == 0

    def test_if_delete_over_times_inserted(self):
        with pytest.raises(SystemError):
            tree = BSTree()
            for i in range(10):
                tree.insert(0)
            for i in range(11):
                tree.delete(0)


class TestRBTreeMinMax:
    def test_min(self):
        for i in range(100):
            tree = BSTree()
            li = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            for val in li:
                tree.insert(val)
            assert tree.min() == min(li)

    def test_max(self):
        for i in range(100):
            tree = BSTree()
            li = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            for val in li:
                tree.insert(val)
            assert tree.max() == max(li)
