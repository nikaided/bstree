from datetime import datetime

from bstree import BSTree
from mock_object import LTObj


def test_intobj():
    tree = BSTree()
    tree.insert(5)
    tree.insert(3)
    tree.insert(7)
    assert tree.to_list() == [3, 5, 7]


def test_ltobj():
    tree = BSTree()
    tree.insert(LTObj(5))
    tree.insert(LTObj(3))
    tree.insert(LTObj(7))
    assert [obj.val for obj in tree.to_list()] == [3, 5, 7]


def test_datetimeobj():
    tree = BSTree()
    tree.insert(datetime(2020, 1, 5))
    tree.insert(datetime(2020, 1, 3))
    tree.insert(datetime(2020, 1, 7))
    assert tree.to_list() == [datetime(2020, 1, 3), datetime(2020, 1, 5), datetime(2020, 1, 7)]


test_ltobj()
