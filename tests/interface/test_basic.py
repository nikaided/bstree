from datetime import datetime

from bstree import BSTree
from tests.mock_object import LTObj


def test_intobj():
    tree = BSTree()
    tree.insert(5)
    tree.insert(3)
    tree.insert(7)
    if tree.has(5):
        tree.delete(5)
    assert tree.to_list() == [3, 7]

def test_ltobj():
    tree = BSTree()
    tree.insert(LTObj(5))
    tree.insert(LTObj(3))
    tree.insert(LTObj(7))
    if tree.has(LTObj(5)):
        tree.delete(LTObj(5))
    assert [obj.val for obj in tree.to_list()] == [3, 7]

def test_datetimeobj():
    tree = BSTree()
    tree.insert(datetime(2020, 1, 5))
    tree.insert(datetime(2020, 1, 3))
    tree.insert(datetime(2020, 1, 7))
    if tree.has(datetime(2020, 1, 5)):
        tree.delete(datetime(2020, 1, 5))
    assert tree.to_list() == [datetime(2020, 1, 3), datetime(2020, 1, 7)]

def test_intobj_with_key():
    tree = BSTree(key=lambda x: abs(x))
    tree.insert(-5)
    tree.insert(-3)
    tree.insert(-7)
    if tree.has(-5):
        tree.delete(-5)
    assert tree.to_list() == [-3, -7]

def test_ltobj_with_key():
    tree = BSTree(key=lambda x: abs(x.val))
    tree.insert(LTObj(-5))
    tree.insert(LTObj(-3))
    tree.insert(LTObj(-7))
    if tree.has(LTObj(-5)):
        tree.delete(LTObj(-5))
    assert [obj.val for obj in tree.to_list()] == [-3, -7]

def test_datetimeobj_with_key():
    tree = BSTree(key=lambda x: x.year)
    tree.insert(datetime(2021, 1, 5))
    tree.insert(datetime(2022, 1, 3))
    tree.insert(datetime(2020, 1, 7))
    if tree.has(datetime(2020, 1, 5)):
        tree.delete(datetime(2021, 1, 5))
    assert tree.to_list() == [datetime(2020, 1, 7), datetime(2022, 1, 3)]
