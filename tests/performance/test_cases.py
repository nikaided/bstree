from datetime import datetime, timezone
import os
from bisect import insort_left, bisect_left
import array

import numpy as np
import pytest

from bstree import BSTree
from tests.mock_io import load_input_txt, load_output_txt
from tests.mock_object import LTObj, GTObj, LTGTObj


def process_generator(algorithm, target_type: str):
    data_dir = os.path.join(os.getcwd(), "data")
    in_dir = os.path.join(data_dir, "in")
    out_dir = os.path.join(data_dir, "out")
    for fname in os.listdir(in_dir):
        input_fpath = os.path.join(in_dir, fname)
        output_fpath = os.path.join(out_dir, fname)
        L, Q, C, X = load_input_txt(input_fpath)
        zero = 0

        if target_type.lower() == "float":
            zero = 0.0
            L = float(L)
            X = [float(val) for val in X]
        elif target_type.lower() == "ltobj":
            zero = LTObj(0)
            L = LTObj(L)
            X = [LTObj(val) for val in X]
        elif target_type.lower() == "gtobj":
            zero = GTObj(0)
            L = GTObj(L)
            X = [GTObj(val) for val in X]
        elif target_type.lower() == "ltgtobj":
            zero = LTGTObj(0)
            L = LTGTObj(L)
            X = [LTGTObj(val) for val in X]
        elif target_type.lower() == "datetime":
            zero = datetime.fromtimestamp(0)
            L = datetime.fromtimestamp(L)
            X = [datetime.fromtimestamp(val) for val in X]

        actual = algorithm(zero, L, C, X, target_type)

        if target_type == "datetime":
            actual = [(datetime(1970, 1, 1, tzinfo=timezone.utc) + val).timestamp() for val in actual]

        expected = load_output_txt(output_fpath)
        yield expected, actual


class Algorithm:
    def __init__(self, key=None):
        self.keyfunc = key

    def bisect_with_list(self, zero, L, C, X, data_type):
        signs = []
        ans = []
        get_key = self.keyfunc if self.keyfunc else lambda x: x
        insort_left(signs, zero, key=self.keyfunc)
        insort_left(signs, L, key=self.keyfunc)
        for c, x in zip(C, X):
            if c == 1:
                idx = insort_left(signs, x, key=self.keyfunc)
            else:
                idx = bisect_left(signs, get_key(x), key=self.keyfunc)
                ans.append(signs[idx] - signs[idx - 1])
        return ans

    def bisect_with_array(self, zero, L, C, X, data_type):
        if data_type == "int":
            signs = array.array("i", [])
        elif data_type == "float":
            signs = array.array("d", [])
        else:
            raise NotImplementedError
        ans = []
        get_key = self.keyfunc if self.keyfunc else lambda x: x
        insort_left(signs, zero, key=self.keyfunc)
        insort_left(signs, L, key=self.keyfunc)
        for c, x in zip(C, X):
            if c == 1:
                idx = insort_left(signs, x, key=self.keyfunc)
            else:
                idx = bisect_left(signs, get_key(x), key=self.keyfunc)
                ans.append(signs[idx] - signs[idx - 1])
        return ans

    def bisect_with_ndarray(self, zero, L, C, X, data_type):
        if data_type == "int":
            signs = np.array([], dtype=np.int32)
        elif data_type == "datetime":
            signs = np.array([], dtype=np.datetime64)
        else:
            signs = np.array([], dtype=object)
        ans = []
        sorter = np.argsort([self.keyfunc(x) for x in signs])
        np.searchsorted(signs, zero, key=sorter)
        np.searchsorted(signs, L, key=sorter)
        for c, x in zip(C, X):
            if c == 1:
                idx = np.searchsorted(signs, x, key=sorter)
                signs = np.insert(signs, idx, x)
            else:
                idx = bisect_left(signs, x, key=self.keyfunc)
                ans.append(signs[idx] - signs[idx - 1])
        return ans

    def bstree(self, zero, L, C, X, data_type):
        tree = BSTree(key=self.keyfunc)
        tree.insert(zero)
        tree.insert(L)
        ans = list()
        for c, x in zip(C, X):
            if c == 1:
                tree.insert(x)
            else:
                ans.append(tree.next_to(x) - tree.prev_to(x))
        return ans


class TestInt:
    def _test_bisect_int_with_list(self):
        al = Algorithm()
        for expected, actual in process_generator(al.bisect_with_list, "int"):
            assert expected == actual

    def _test_bisect_int_with_array(self):
        al = Algorithm()
        for expected, actual in process_generator(al.bisect_with_array, "int"):
            assert expected == actual

    def _test_bisect_int_with_ndarray(self):
        al = Algorithm()
        for expected, actual in process_generator(al.bisect_with_ndarray, "int"):
            assert expected == actual

    def test_bstree_int(self):
        al = Algorithm()
        for expected, actual in process_generator(al.bstree, "int"):
            assert expected == actual


class TestIntWithKey:
    def _test_bisect_int_with_list(self):
        al = Algorithm(key=abs)
        for expected, actual in process_generator(al.bisect_with_list, "int"):
            assert expected == actual

    def _test_bisect_int_with_array(self):
        al = Algorithm(key=abs)
        for expected, actual in process_generator(al.bisect_with_array, "int"):
            assert expected == actual

    def _test_bisect_int_with_ndarray(self):
        al = Algorithm(key=abs)
        for expected, actual in process_generator(al.bisect_with_ndarray, "int"):
            assert expected == actual

    def test_bstree_int(self):
        al = Algorithm(key=abs)
        for expected, actual in process_generator(al.bstree, "int"):
            assert expected == actual


class TestFloat:
    def _test_bisect_float_with_list(self):
        al = Algorithm()
        for expected, actual in process_generator(al.bisect_with_list, "float"):
            assert expected == actual

    def _test_bisect_float_with_array(self):
        al = Algorithm()
        for expected, actual in process_generator(al.bisect_with_array, "float"):
            assert expected == actual

    def _test_bisect_float_with_ndarray(self):
        al = Algorithm()
        for expected, actual in process_generator(al.bisect_with_ndarray, "float"):
            assert expected == actual

    def test_bstree_float(self):
        al = Algorithm()
        for expected, actual in process_generator(al.bstree, "float"):
            assert expected == actual


class TestDatetime:
    def _test_bisect_datetime_with_list(self):
        al = Algorithm()
        for expected, actual in process_generator(al.bisect_with_list, "datetime"):
            assert expected == actual

    def _test_bisect_datetime_with_ndarray(self):
        al = Algorithm()
        for expected, actual in process_generator(al.bisect_with_ndarray, "datetime"):
            assert expected == actual

    def test_bstree_datetime(self):
        al = Algorithm()
        for expected, actual in process_generator(al.bstree, "datetime"):
            assert expected == actual


class TestLTObj:
    def _test_bisect_ltobj_with_list(self):
        al = Algorithm()
        for expected, actual in process_generator(al.bisect_with_list, "ltobj"):
            assert expected == actual

    def _test_bisect_ltobj_with_ndarray(self):
        al = Algorithm()
        for expected, actual in process_generator(al.bisect_with_ndarray, "ltobj"):
            assert expected == actual

    def test_bstree_ltobj(self):
        al = Algorithm()
        for expected, actual in process_generator(al.bstree, "ltobj"):
            assert expected == actual


class TestLTObjWithKey:
    def _test_bisect_ltobj_with_list(self):
        al = Algorithm(key=lambda x: abs(x.val))
        for expected, actual in process_generator(al.bisect_with_list, "ltobj"):
            assert expected == actual

    def _test_bisect_ltobj_with_ndarray(self):
        al = Algorithm(key=lambda x: abs(x.val))
        for expected, actual in process_generator(al.bisect_with_ndarray, "ltobj"):
            assert expected == actual

    def test_bstree_ltobj(self):
        al = Algorithm(key=lambda x: abs(x.val))
        for expected, actual in process_generator(al.bstree, "ltobj"):
            assert expected == actual
