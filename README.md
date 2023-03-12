# bstree
二分探索木
内部ではRed Black Treeを実装している
treap実装を追加予定

```shell
pip install bstree
```

```python
from bstree import BSTree

bst = BSTree()
# 要素の追加
bst.insert(1)
# 要素の探索
bst.search(1)
# 要素の削除
bst.delete(1)

for i in range(100):
    bst.insert(random.randomint())
# 要素の表示
print(bst)
# k番目に小さい値の表示(k=1以外未実装)
bst.min(k)
# k番目に大きい値の表示(k=1以外未実装)
bst.max(k)
```
