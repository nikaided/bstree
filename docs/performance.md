## Performance

Here is a performance comparison of bstree with the existing bisect library:

| Operation           | BSTree (Standard) | Bisect                  |
| ------------------- | ----------------- | ----------------------- |
| Insert              | O(log N)          | O(log N)                |
| Search              | O(log N)          | O(log N)                |
| Delete              | O(log N)          | O(log N)                |
| k-th Smallest       | O(log N)          | O(log N)                |
| Rank Calculation    | O(log N)          | O(log N)                |
| Traversal (Ordered) | O(N)              | O(N)                    |

Note: The Bisect variant may offer additional optimizations for specific use cases or larger datasets.
