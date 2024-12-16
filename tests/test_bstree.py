from datetime import datetime
import sys
from bisect import bisect_left
from collections import Counter
from random import randint, choice, shuffle, sample

import pytest

from bstree import BSTree
from mock_object import LTObj


class TestRBTreeInit:
    def test_type_error_when_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree(0, 0)

    def test_init_size(self):
        tree = BSTree()
        assert tree.size == 0

    def test_init_list(self):
        tree = BSTree()
        assert tree.to_list() == []

    def test_init_counter(self):
        tree = BSTree()
        assert tree.to_counter() == {}

    def test_argument(self):
        tree = BSTree(True)
        tree = BSTree(False)
        assert True

    def test_keyword_argument(self):
        tree = BSTree(dup=True)
        tree = BSTree(dup=False)
        assert True

    def test_keyword_tricky_argument(self):
        tree = BSTree(1)
        tree = BSTree(0)
        tree = BSTree(2)
        tree = BSTree(-1)
        tree = BSTree(0 == 0)
        tree = BSTree(0 == 1)
        assert True

    def test_if_argument_is_float(self):
        """want to raise exception but not satisfied yet
        because python evauates non-zero value as True"""
        with pytest.raises(TypeError):
            tree = BSTree(0.1)


class TestRBTreeClear:

    def test_size_when_clear_intObj(self):
        tree = BSTree()
        for i in range(10**3):
            tree.insert(i)
        tree.clear()
        assert tree.size == 0

    def test_size_when_clear_floatObj(self):
        tree = BSTree()
        for i in range(1):
            tree.insert(float(i))
        tree.clear()
        assert tree.size == 0

    def test_size_when_clear_LTObj(self):
        tree = BSTree()
        for i in range(10**3):
            tree.insert(LTObj(i))
        tree.clear()
        assert tree.size == 0

    def test_size_clear_datetimeObj(self):
        tree = BSTree()
        for i in range(10**3):
            tree.insert(datetime.now())
        tree.clear()
        assert tree.size == 0


class TestRBTreeInsert:
    def test_size_when_insert_random_value(self):
        for i in range(100):
            tree = BSTree()
            insert_list = [randint(-pow(2, 7), pow(2, 7)) for i in range(100)]
            for val in insert_list:
                tree.insert(val)
            assert tree.size == len(set(insert_list))

    def test_size_when_insert_random_value_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(dup=True)
            insert_list = [randint(-pow(2, 7), pow(2, 7)) for i in range(100)]
            counter = {}
            for val in insert_list:
                tree.insert(val)
            assert tree.size == len(insert_list)

    def test_counter_when_insert_random_value(self):
        for i in range(100):
            tree = BSTree()
            insert_list = [randint(-pow(2, 7), pow(2, 7)) for i in range(100)]
            counter = {}
            for val in insert_list:
                tree.insert(val)
                counter[val] = 1
            assert tree.to_counter() == counter

    def test_counter_when_insert_random_value_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(dup=True)
            insert_list = [randint(-pow(2, 7), pow(2, 7)) for i in range(100)]
            counter = {}
            for val in insert_list:
                tree.insert(val)
                if val in counter:
                    counter[val] += 1
                else:
                    counter[val] = 1
            assert tree.to_counter() == counter

    def test_counter_when_any_unhashable_in_tree(self):
        # [TODO investigate why this test is failing]
        with pytest.raises(TypeError):
            tree = BSTree()
            n = randint(1, 100)
            for j in range(n):
                tree.insert(LTObj(j))
            tree.to_counter()

    def test_size_when_insert_same_value(self):
        tree = BSTree()
        n = randint(1, 100)
        for j in range(n):
            tree.insert(0)
        assert tree.size == 1

    def test_size_when_insert_same_value_when_dup_is_true(self):
        tree = BSTree(dup=True)
        n = randint(0, 100)
        for j in range(n):
            tree.insert(0)
        assert tree.size == n

    def test_when_insert_system_max_value(self):
        tree = BSTree()
        tree.insert(sys.maxsize)
        assert True


class TestRBTreeToList:
    def test_type_error_when_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.to_list(0)

    def test_tolist_when_tree_is_empty(self):
        tree = BSTree()
        tree.to_list()
        assert True

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

    def test_order(self):
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

    def test_reverse_order(self):
        for i in range(100):
            tree = BSTree()
            n = randint(0, 100)
            insert_list = [randint(-pow(2, 7), pow(2, 7)) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = list(set(insert_list))
            expected.sort(reverse=True)
            actual = tree.to_list(reverse=True)
            assert expected == actual

    def test_order_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(dup=True)
            n = randint(0, 100)
            insert_list = [randint(-pow(2, 7), pow(2, 7)) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = insert_list.copy()
            expected.sort()
            actual = tree.to_list()
            assert expected == actual

    def test_reverse_order_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(dup=True)
            n = randint(0, 100)
            insert_list = [randint(-pow(2, 7), pow(2, 7)) for i in range(n)]
            for val in insert_list:
                tree.insert(val)
            expected = insert_list.copy()
            expected.sort(reverse=True)
            actual = tree.to_list(reverse=True)
            assert expected == actual

    def test_order_when_delete_randomly(self):
        for i in range(100):
            tree = BSTree()
            test_set = set()
            m = 100
            insert_list = [randint(-pow(2, 4), pow(2, 4)) for i in range(m)]
            for val in insert_list:
                tree.insert(val)
                test_set.add(val)
            while test_set:
                element = sample(list(test_set), 1)[0]
                tree.delete(element)
                test_set.remove(element)
                assert tree.to_list() == sorted(test_set)

    def test_reverse_order_when_delete_randomly(self):
        for i in range(100):
            tree = BSTree()
            test_set = set()
            m = 100
            insert_list = [randint(-pow(2, 4), pow(2, 4)) for i in range(m)]
            for val in insert_list:
                tree.insert(val)
                test_set.add(val)
            while test_set:
                element = sample(list(test_set), 1)[0]
                tree.delete(element)
                test_set.remove(element)
                assert tree.to_list(reverse=True) == sorted(test_set, reverse=True)

    def test_reverse_order_when_deleted_random_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(dup=True)
            test_list = []
            m = 100
            insert_list = [randint(-pow(2, 4), pow(2, 4)) for i in range(m)]
            for val in insert_list:
                tree.insert(val)
                test_list.append(val)
            while test_list:
                element = sample(test_list, 1)[0]
                tree.delete(element)
                test_list.remove(element)
                assert tree.to_list(reverse=True) == sorted(test_list, reverse=True)

    def test_order_when_deleted_random_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(dup=True)
            test_list = []
            m = 100
            insert_list = [randint(-pow(2, 4), pow(2, 4)) for i in range(m)]
            for val in insert_list:
                tree.insert(val)
                test_list.append(val)
            while test_list:
                element = sample(test_list, 1)[0]
                tree.delete(element)
                test_list.remove(element)
                assert tree.to_list() == sorted(test_list)


class TestRBTreeSearch:
    def test_type_error_when_no_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.has()

    def test_type_error_when_two_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.has(0, 1)

    def test_search_inserted(self):
        tree = BSTree()
        tree.insert(0)
        assert tree.has(0)

    def test_search_not_inserted(self):
        tree = BSTree()
        tree.insert(0)
        assert not tree.has(1)

    def test_search_deleted(self):
        tree = BSTree()
        tree.insert(0)
        tree.delete(0)
        assert not tree.has(0)

    def test_search_deleted_when_dup_is_true(self):
        tree = BSTree(dup=True)
        tree.insert(0)
        tree.delete(0)
        assert not tree.has(0)

    def test_search_deleted2(self):
        tree = BSTree()
        tree.insert(0)
        tree.insert(0)
        tree.delete(0)
        assert not tree.has(0)

    def test_search_deleted_when_dup_is_true2(self):
        tree = BSTree(True)
        tree.insert(0)
        tree.insert(0)
        tree.delete(0)
        assert tree.has(0)


class TestRBTreeDelete:
    def test_type_error_when_no_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.delete()

    def test_type_error_when_two_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.delete(0, 1)

    def test_delete(self):
        tree = BSTree()
        tree.insert(0)
        tree.delete(0)
        assert True

    def test_delete_when_dup_is_true(self):
        tree = BSTree(dup=True)
        tree.insert(0)
        tree.delete(0)
        assert True

    def test_when_delete_what_is_not_inserted(self):
        with pytest.raises(ValueError):
            tree = BSTree()
            tree.delete(0)

    def test_size_when_delete_randomly(self):
        for i in range(100):
            tree = BSTree()
            test_set = set()
            m = 100
            insert_list = [randint(-pow(2, 4), pow(2, 4)) for i in range(m)]
            for val in insert_list:
                tree.insert(val)
                test_set.add(val)
            while test_set:
                element = sample(list(test_set), 1)[0]
                tree.delete(element)
                test_set.remove(element)
                assert tree.size == len(test_set)

    def test_counter_when_delete_randomly(self):
        for i in range(100):
            tree = BSTree()
            test_set = set()
            m = 100
            insert_list = [randint(-pow(2, 4), pow(2, 4)) for i in range(m)]
            for val in insert_list:
                tree.insert(val)
                test_set.add(val)

            while test_set:
                element = sample(list(test_set), 1)[0]
                tree.delete(element)
                test_set.remove(element)
                assert tree.to_counter() == dict(Counter(test_set))

    def test_size_when_delete_randomly_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(dup=True)
            test_list = []
            m = 100
            insert_list = [randint(-pow(2, 4), pow(2, 4)) for i in range(m)]
            for val in insert_list:
                tree.insert(val)
                test_list.append(val)
            while test_list:
                element = sample(test_list, 1)[0]
                tree.delete(element)
                test_list.remove(element)
                assert tree.size == len(test_list)

    def test_counter_when_delete_randomly_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(dup=True)
            test_list = []
            m = 100
            insert_list = [randint(-pow(2, 4), pow(2, 4)) for i in range(m)]
            for val in insert_list:
                tree.insert(val)
                test_list.append(val)
            while test_list:
                element = sample(test_list, 1)[0]
                tree.delete(element)
                test_list.remove(element)
                assert tree.to_counter() == dict(Counter(test_list))


class TestRBTreeNextPrev:
    def test_type_error_when_no_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.next_to()

    def test_type_error_when_two_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.next_to(0, 1)

    def test_get_next(self):
        for i in range(100):
            tree = BSTree()
            test_set = set()
            insert_list = [randint(-1000, 1000) for i in range(100)]
            for val in insert_list:
                tree.insert(val)
                test_set.add(val)
            idx = randint(0, len(test_set) - 1)
            sorted_list = sorted(test_set)
            sorted_list.append(None)
            assert tree.next_to(sorted_list[idx]) == sorted_list[idx + 1]

    def test_get_next_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(dup=True)
            test_list = list()
            insert_list = [randint(-1000, 1000) for i in range(100)]
            for val in insert_list:
                tree.insert(val)
                test_list.append(val)
            idx = randint(0, len(test_list) - 2)
            sorted_list = sorted(test_list)
            sorted_list.append(None)
            inc = 1
            while sorted_list[idx] == sorted_list[idx + inc]:
                inc += 1
            assert tree.next_to(sorted_list[idx]) == sorted_list[idx + inc]

    def test_get_next_when_tree_is_empty(self):
        tree = BSTree()
        assert tree.next_to(0) is None

    def test_get_next_when_key_is_max(self):
        tree = BSTree(True)
        tree.insert(0)
        assert tree.next_to(0) is None

    def test_get_next_when_key_is_not_inserted(self):
        tree = BSTree()
        tree.insert(0)
        tree.insert(2)
        assert tree.next_to(1) == 2

    def test_get_next_when_key_is_not_inserted2(self):
        tree = BSTree()
        tree.insert(0)
        tree.insert(2)
        assert tree.next_to(3) is None


class TestRBTreePrev:
    def test_type_error_when_no_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.prev_to()

    def test_type_error_when_two_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.prev_to(0, 1)

    def test_get_prev(self):
        for i in range(100):
            tree = BSTree()
            test_set = set()
            insert_list = [randint(-1000, 1000) for i in range(100)]
            for i in insert_list:
                tree.insert(i)
                test_set.add(i)
            idx = randint(1, len(test_set))
            sorted_list = sorted(test_set)
            sorted_list = [None] + sorted_list
            assert tree.prev_to(sorted_list[idx]) == sorted_list[idx - 1]

    def test_get_prev_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(dup=True)
            test_list = list()
            insert_list = [randint(-1000, 1000) for i in range(100)]
            for val in insert_list:
                tree.insert(val)
                test_list.append(val)
            idx = randint(1, len(test_list))
            sorted_list = sorted(test_list)
            sorted_list = [None] + sorted_list
            dec = 1
            while sorted_list[idx] == sorted_list[idx - dec]:
                dec += 1
            assert tree.prev_to(sorted_list[idx]) == sorted_list[idx - dec]

    def test_get_prev_when_tree_is_empty(self):
        tree = BSTree()
        assert tree.prev_to(0) is None

    def test_get_prev_when_key_is_min(self):
        tree = BSTree(True)
        tree.insert(0)
        assert tree.prev_to(0) is None

    def test_get_prev_when_key_is_not_inserted(self):
        tree = BSTree()
        tree.insert(0)
        tree.insert(2)
        assert tree.prev_to(1) == 0

    def test_get_prev_when_key_is_not_inserted2(self):
        tree = BSTree()
        tree.insert(0)
        tree.insert(2)
        assert tree.prev_to(-1) is None


class TestRBTreeKthSmallest:
    def test_type_error_when_two_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.kth_smallest(0, 1)

    def test_smallest(self):
        for i in range(100):
            tree = BSTree()
            insert_list = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            for val in insert_list:
                tree.insert(val)
            assert tree.kth_smallest() == min(insert_list)

    def test_smallest_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(True)
            insert_list = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            for val in insert_list:
                tree.insert(val)
            assert tree.kth_smallest() == min(insert_list)

    def test_get_kth_smallest(self):
        for i in range(100):
            tree = BSTree()
            insert_list = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            test_set = set()
            for val in insert_list:
                tree.insert(val)
                test_set.add(val)
            test_list = sorted(test_set)
            for k in range(1, len(test_list) + 1):
                assert tree.kth_smallest(k) == test_list[k - 1]

    def test_get_kth_smallest_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(True)
            insert_list = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            test_list = list()
            for val in insert_list:
                tree.insert(val)
                test_list.append(val)
            test_list.sort()
            for k in range(1, 101):
                assert tree.kth_smallest(k) == test_list[k - 1]

    def test_get_kth_smallest_after_deleted(self):
        for i in range(100):
            tree = BSTree()
            insert_list = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            test_set = set()
            for val in insert_list:
                tree.insert(val)
                test_set.add(val)
            while test_set:
                test_list = sorted(test_set)
                k = randint(1, len(test_list))
                assert tree.kth_smallest(k) == test_list[k - 1]
                element = sample(list(test_set), 1)[0]
                tree.delete(element)
                test_set.remove(element)

    def test_get_kth_smallest_after_deleted_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(dup=True)
            insert_list = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            test_list = list()
            for val in insert_list:
                tree.insert(val)
                test_list.append(val)
            while test_list:
                test_list.sort()
                k = randint(1, len(test_list))
                assert tree.kth_smallest(k) == test_list[k - 1]
                element = sample(test_list, 1)[0]
                tree.delete(element)
                test_list.remove(element)

    def test_smallest_when_k_is_out_of_range(self):
        tree = BSTree()
        with pytest.raises(IndexError):
            tree.kth_smallest()


class TestRBTreeKthLargest:
    def test_type_error_when_two_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.kth_largest(0, 1)

    def test_largest(self):
        for i in range(100):
            tree = BSTree()
            insert_list = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            for val in insert_list:
                tree.insert(val)
            assert tree.kth_largest() == max(insert_list)

    def test_largest_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(True)
            insert_list = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            for val in insert_list:
                tree.insert(val)
            assert tree.kth_largest() == max(insert_list)

    def test_get_kth_largest(self):
        for i in range(100):
            tree = BSTree()
            insert_list = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            test_set = set()
            for val in insert_list:
                tree.insert(val)
                test_set.add(val)
            test_list = sorted(test_set, reverse=True)
            for k in range(1, len(test_list) + 1):
                assert tree.kth_largest(k) == test_list[k - 1]

    def test_get_kth_largest_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(True)
            insert_list = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            test_list = list()
            for val in insert_list:
                tree.insert(val)
                test_list.append(val)
            test_list.sort(reverse=True)
            for k in range(1, 101):
                assert tree.kth_largest(k) == test_list[k - 1]

    def test_get_kth_largest_after_deleted(self):
        for i in range(100):
            tree = BSTree()
            insert_list = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            test_set = set()
            for val in insert_list:
                tree.insert(val)
                test_set.add(val)
            while test_set:
                test_list = sorted(test_set, reverse=True)
                k = randint(1, len(test_list))
                assert tree.kth_largest(k) == test_list[k - 1]
                element = sample(list(test_set), 1)[0]
                tree.delete(element)
                test_set.remove(element)

    def test_get_kth_largest_after_deleted_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(dup=True)
            insert_list = [randint(-pow(10, 3), pow(10, 3)) for j in range(100)]
            test_list = list()
            for val in insert_list:
                tree.insert(val)
                test_list.append(val)
            while test_list:
                test_list.sort(reverse=True)
                k = randint(1, len(test_list))
                assert tree.kth_largest(k) == test_list[k - 1]
                element = sample(test_list, 1)[0]
                tree.delete(element)
                test_list.remove(element)

    def test_largest_when_k_is_out_of_range(self):
        tree = BSTree()
        with pytest.raises(IndexError):
            tree.kth_largest()


class TestRBTreeRank:
    def test_type_error_when_no_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.rank()

    def test_type_error_when_two_arguments(self):
        with pytest.raises(TypeError):
            tree = BSTree()
            tree.rank(0, 1)

    def test_rank(self):
        tree = BSTree()
        li = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
        for val in li:
            tree.insert(val)
        assert tree.rank(0) == 0
        assert tree.rank(1) == 1
        assert tree.rank(100) == 10

    def test_rank_when_dup_is_true(self):
        tree = BSTree(dup=True)
        li = [0 for i in range(10)]
        for val in li:
            tree.insert(val)
        assert tree.rank(0) == 0
        assert tree.rank(1) == 10
        assert tree.rank(100) == 10

    def test_rank_when_insert_randomly(self):
        for i in range(100):
            tree = BSTree()
            insert_list = [randint(-100, 100) for j in range(100)]
            test_set = set()
            for val in insert_list:
                tree.insert(val)
                test_set.add(val)
            k = randint(-200, 200)
            actual = tree.rank(k)
            test_list = sorted(test_set)
            expected = bisect_left(test_list, k)
            assert expected == actual

    def test_rank_when_insert_randomly_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(True)
            insert_list = [randint(-100, 100) for j in range(100)]
            test_list = list()
            for val in insert_list:
                tree.insert(val)
                test_list.append(val)
            k = randint(-200, 200)
            actual = tree.rank(k)
            test_list.sort()
            expected = bisect_left(test_list, k)
            assert expected == actual

    def test_rank_when_delete_randomly(self):
        for i in range(100):
            tree = BSTree()
            insert_list = [randint(-100, 100) for j in range(100)]
            test_set = set()
            for val in insert_list:
                tree.insert(val)
                test_set.add(val)
            while test_set:
                element = sample(list(test_set), 1)[0]
                tree.delete(element)
                test_set.remove(element)

                k = randint(-200, 200)
                actual = tree.rank(k)
                test_list = sorted(test_set)
                expected = bisect_left(test_list, k)
                assert expected == actual

    def test_rank_when_delete_randomly_when_dup_is_true(self):
        for i in range(100):
            tree = BSTree(True)
            insert_list = [randint(-100, 100) for j in range(100)]
            test_list = list()
            for val in insert_list:
                tree.insert(val)
                test_list.append(val)
            while test_list:
                element = sample(test_list, 1)[0]
                tree.delete(element)
                test_list.remove(element)

                k = randint(-200, 200)
                actual = tree.rank(k)
                test_list.sort()
                expected = bisect_left(test_list, k)
                assert expected == actual
