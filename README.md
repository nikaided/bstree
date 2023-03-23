# what this module is
It provides you "Binary Search Tree" data structure.

- It inserts, searches, and deletes some data in O(log(N)) time complexity.
- Also provides the method of searching n-th smallest value, and getting the rank of a value both in O(log(N)) time complexity.

- RBTree structure is implemented inside.
- Treap structure will be implemented soon.

# How to install
```shell
pip install bstree
```

# Basic Usage
```python
from bstree import BSTree
# create an object
# some options are available
# model option should be "rbtree" or "treap"
# default model is "bstree"
# dup option should be True if duplicated value is permitted else False
# default dup value is False
bst = BSTree()
bst = BSTree(model="treap", dup=True)

# insert an element
# should be integer for the moment
bst.insert(1)
# search an element
# if it is in the tree, return True else False
if bst.search(1):
    # delete an element
    bst.delete(1)

for i in range(100):
    bst.insert(random.randomint())
# represent the size of the tree.
print(bst.size)
# represent the whole elements in the tree as a list in order.
print(bst.to_list())
# get the next(previous) value of k if k is in the tree
print(bst.next(k))
print(bst.prev(k))
# get the k-th smallest/largest value
# default k is 1
print(bst.kth_smallest(k))
print(bst.kth_largest(k))
# get the number of elements which is strictly less than n.
print(bst.rank_of(n))
```
