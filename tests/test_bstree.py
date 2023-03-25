import sys
from bisect import bisect_left
from random import randint, choice, shuffle
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


class TestRBTreeNextPrev:
    def test_get_next(self):
        tree = BSTree()
        input_list = list(range(100))
        shuffle(input_list)
        for i in input_list:
            tree.insert(i)
        for key in range(99):
            assert tree.next_to(key) == key + 1

    def test_get_prev(self):
        tree = BSTree()
        input_list = list(range(100))
        shuffle(input_list)
        for i in input_list:
            tree.insert(i)
        for key in range(1, 100):
            assert tree.prev_to(key) == key - 1

    def test_get_next_when_tree_is_empty(self):
        tree = BSTree()
        assert tree.next_to(0) == None

    def test_get_prev_when_tree_is_empty(self):
        tree = BSTree()
        assert tree.prev_to(0) == None

    def test_get_next_when_key_is_max(self):
        tree = BSTree()
        tree.insert(0)
        assert tree.next_to(0) == None

    def test_get_prev_when_key_is_max(self):
        tree = BSTree()
        tree.insert(0)
        assert tree.prev_to(0) == None

    def test_get_next_when_key_is_not_inserted(self):
        tree = BSTree()
        input_list = list(range(0, 100, 2))
        shuffle(input_list)
        for i in input_list:
            tree.insert(i)
        for i in range(100):
            k = randint(0, 49) * 2 - 1
            assert tree.next_to(k) == k + 1

    def test_get_prev_when_key_is_not_inserted(self):
        tree = BSTree()
        input_list = list(range(0, 100, 2))
        shuffle(input_list)
        for i in input_list:
            tree.insert(i)
        for i in range(100):
            k = randint(0, 49) * 2 + 1
            assert tree.prev_to(k) == k - 1


class TestRBTreeKthSmallestLargest:
    def test_smallest(self):
        for i in range(100):
            tree = BSTree()
            li = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            for val in li:
                tree.insert(val)
            assert tree.kth_smallest() == min(li)

    def test_get_kth_smallest(self):
        for i in range(100):
            tree = BSTree()
            li = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            for val in li:
                tree.insert(val)
            li.sort()
            k = randint(1, 100)
            assert tree.kth_smallest(k) == li[k - 1]

    def test_smallest_when_k_is_out_of_range(self):
        tree = BSTree()
        tree.insert(0)
        with pytest.raises(IndexError):
            tree.kth_smallest(2)

    def test_largest(self):
        for i in range(100):
            tree = BSTree()
            li = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            for val in li:
                tree.insert(val)
            assert tree.kth_largest() == max(li)

    def test_get_kth_smallest(self):
        for i in range(100):
            tree = BSTree()
            li = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            for val in li:
                tree.insert(val)
            li.sort(reverse=True)
            k = randint(1, 100)
            assert tree.kth_largest(k) == li[k - 1]

    def test_largest_when_k_is_out_of_range(self):
        tree = BSTree()
        tree.insert(0)
        with pytest.raises(IndexError):
            tree.kth_largest(2)


class TestRBTreeRank:
    def test_get_the_rank(self):
        tree = BSTree()
        li = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
        for val in li:
            tree.insert(val)
        assert tree.rank(0) == 0
        assert tree.rank(1) == 1
        assert tree.rank(100) == 10

    def test_when_multiple_vals(self):
        tree = BSTree()
        li = [0 for i in range(10)]
        for val in li:
            tree.insert(val)
        assert tree.rank(0) == 0
        assert tree.rank(1) == 10
        assert tree.rank(100) == 10

    def test_random(self):
        for i in range(100):
            tree = BSTree()
            li = [randint(-100, 100) for j in range(100)]
            for val in li:
                tree.insert(val)
            k = randint(-200, 200)
            actual = tree.rank(k)
            li.sort()
            expected = bisect_left(li, k)
            assert expected == actual
