# what this module is
- It provides you "Binary Search Tree" data structure.
- It searches, and deletes some data in O(log(N)) time complexity.
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

bst = BSTree()
# insert an element
bst.insert(1)
# search an element
bst.search(1)
# delete an element
bst.delete(1)

for i in range(100):
    bst.insert(random.randomint())
# represent the whole elements in the tree as a list in order.
bst.to_list()
# get the k-th smallest value
bst.min(k)
# get the number of elements which is less than n.
bst.rank(n)
```
