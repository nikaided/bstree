import timeit

from test_cases import TestInt, TestFloat, TestLTObj, TestDatetime

class TestPerformanceInt:
    def test_performance_bstree_int(self):
        ts = TestInt()
        bstree_sec = timeit.timeit(stmt=ts.test_bstree_int, number=1)
        bisect_int_with_list_sec = timeit.timeit(stmt=ts._test_bisect_int_with_list, number=1)
        bisect_int_with_array_sec = timeit.timeit(stmt=ts._test_bisect_int_with_array, number=1)
        # bisect_int_with_ndarray_sec = timeit.timeit(stmt=ts._test_bisect_int_with_ndarray, number=1)
        assert bstree_sec < bisect_int_with_list_sec
        assert bstree_sec < bisect_int_with_array_sec
        # assert bstree_sec < bisect_int_with_ndarray_sec
        assert bstree_sec < 3

class TestPerformanceFloat:
    def test_performance_bstree_float(self):
        ts = TestFloat()
        bstree_sec = timeit.timeit(stmt=ts.test_bstree_float, number=1)
        bisect_float_with_list_sec = timeit.timeit(stmt=ts._test_bisect_float_with_list, number=1)
        bisect_float_with_array_sec = timeit.timeit(stmt=ts._test_bisect_float_with_array, number=1)
        # bisect_float_with_ndarray_sec = timeit.timeit(stmt=ts._test_bisect_float_with_ndarray, number=1)
        assert bstree_sec < bisect_float_with_list_sec
        assert bstree_sec < bisect_float_with_array_sec
        # assert bstree_sec < bisect_float_with_ndarray_sec
        assert bstree_sec < 3


class TestPerformanceLTObj:
    def test_performance_bstree_ltobj(self):
        ts = TestLTObj()
        bstree_sec = timeit.timeit(stmt=ts.test_bstree_ltobj, number=1)
        bisect_ltobj_with_list_sec = timeit.timeit(stmt=ts._test_bisect_ltobj_with_list, number=1)
        # bisect_ltobj_with_ndarray_sec = timeit.timeit(stmt=ts._test_bisect_ltobj_with_ndarray, number=1)
        assert bstree_sec < bisect_ltobj_with_list_sec
        # assert bstree_sec < bisect_ltobj_with_ndarray_sec
        assert bstree_sec < 3


class TestPerformanceDatetime:
    def test_performance_bstree_datetime(self):
        ts = TestDatetime()
        bstree_sec = timeit.timeit(stmt=ts.test_bstree_datetime, number=1)
        bisect_datetime_with_list_sec = timeit.timeit(stmt=ts._test_bisect_datetime_with_list, number=1)
        # bisect_datetime_with_ndarray_sec = timeit.timeit(stmt=ts._test_bisect_datetime_with_ndarray, number=1)
        assert bstree_sec < bisect_datetime_with_list_sec
        # assert bstree_sec < bisect_datetime_with_ndarray_sec
        assert bstree_sec < 3
