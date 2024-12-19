## APIs

The following API methods are available for interacting with the BSTree:

#### `__init__(dup=False, key=None)`

Initializes a new BSTree object.  

**Arguments:**

- `dup` (`bool`, optional, default is `False`): If `True`, the tree will allow duplicate values and treat them as distinct objects. If `False` (default), duplicate values will be treated as a single object.

- `key` (`callable`, optional, default is None): If `None`, the objects themselves are used for comparison. If not `None`, the key function is applied to the objects, and the result is used for comparison.

---
#### `insert(object)`

Insert an object into the tree. In the case dup is False, if the tree already contains an object with the same value (as determined by the key function or the object itself), the object will not be added, and False will be returned.

**Arguments:**

- object (PyObject): The object to be inserted into the tree.


**Return:**

- `True`: The object was successfully inserted into the tree.
- `False`: The object was not inserted because a duplicate value already exists in the tree (only when dup is False).

**Error:**

- Raises `TypeError` if the object is of an invalid type that cannot be compared or inserted into the tree.

---
#### `delete(object)`

Delete an object from the tree which key is the same as the input object. In the case `key` function is set, randomly delete an object in the tree which has the same key as the argument object does.  

**Arguments:**

- `object`

**Return:**  

- `None`

**Error:**  

- Raises `TypeError` if the object cannot be compared (e.g., `key` function is not applicable or objects are not comparable).
- Raises `ValueError` if the object is not in the tree.

---
#### `has(object)`

Check if there is an object in the tree which key is the same as the input object.

**Arguments:**

- `object`

**Return:**  

- `True`: The object is in the tree.
- `False`: The object is not in the tree.

---
#### `to_list(reverse=False)`

Returns a list of objects in the tree, ordered in ascending value.

**Arguments:**

- `reverse` (`bool`, optional): If `True`, the list will be returned in descending order. If `False` (default), the list will be in ascending order.

---
#### `to_counter()`

Returns a counter of the objects in the tree.

**Note:** If the objects in the tree are not hashable, this method will not work correctly.

---
#### `next_to(object)`

Returns the next value greater than the given object.

**Note:** The `object` argument does not need to be an object that exists in the tree.

---
#### `prev_to(object)`

Returns the previous value smaller than the given object.

**Note:** The `object` argument does not need to be an object that exists in the tree.

---
#### `min()`

Returns an arbitrary object in the tree among those with the smallest key.

**Note:** This is equivalent to calling `kth_smallest(0)`, which returns the smallest value in the tree.

**Error:**

- `TypeError`: Raised if there are arguments.

---
#### `max()`

Returns an arbitrary object in the tree among those with the largest key.

**Note:** This is equivalent to calling `kth_largest(0)`, which returns the largest value in the tree.

**Error:**

- `TypeError`: Raised if there are arguments.

---
#### `kth_smallest(k)`

Returns an arbitrary object in the tree among those with the k-th smallest key.

**Arguments:**

- k (`int`): The position (1-indexed) of the desired key in the sorted order of keys. For example, k=1 returns an object with the smallest key.

**Return:**

- An object from the tree that corresponds to the k-th smallest key. If multiple objects share the same key, one of them is returned arbitrarily.

**Error:**

- `ValueError`: Raised if k is less than 1 or greater than the total number of keys in the tree.
- `TypeError`: Raised if k is not an integer.

---
#### `kth_largest(k)`

Returns an arbitrary object in the tree among those with the k-th largest key.

**Arguments:**

- k (`int`): The position (1-indexed) of the desired key in the sorted order of keys. For example, k=1 returns an object with the largest key.

**Return:**

- An object from the tree that corresponds to the k-th largest key. If multiple objects share the same key, one of them is returned arbitrarily.

**Error:**

- `ValueError`: Raised if k is less than 1 or greater than the total number of keys in the tree.
- `TypeError`: Raised if k is not an integer.

---
#### `rank(object)`

Returns the rank of the given object in the tree (its position in order).

**Explanation:** The rank of an object is the number of elements in the tree that are strictly less than the given object.  
 This is equivalent to the behavior of `bisect_left()` in Python's `bisect` module, which returns the position where the object would fit in a sorted list while maintaining the order.

**Arguments:**

- `object` (any): The object whose rank in the tree is to be determined. The object must be comparable with other objects in the tree using the comparison function or key provided during tree initialization.

**Return:**

- `int`: The rank of the object, which is the number of elements in the tree that are strictly less than the given object.

**Error:**

- `TypeError`: Raised if the provided object is of an invalid type.

 ---
 #### `clear()`

 Clear the tree while preserving the duplicate setting.
