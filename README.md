[![Build Status](https://travis-ci.com/nickitat/flov.svg?branch=master)](https://travis-ci.com/nickitat/flov)

# flov
*Dunno whether the following idea is well known or not. But I never heard it before.*

## The problem
We have a sequence of integers, which grows time after time. And sometimes we want to check, if the given integer is already present in our sequence or not. So, we need a data structure supporting operations `Insert(key)` and `Find(key)`.

## The approach
Lets store all the numbers in a vector-like manner: new key is always placed right after the last one.
And for every node we will maintaine the table `link[]` the size of our integer type (in bits).

For the given node `node.link[5]` is defined to be the link to the nearest node (from the right) with a key having exactly the same first 5 bits (from 0-th to 4-th) and which fifth bit is the opposite to the `node.key`'s fifth bit.

## Implementation

### Find

Find is as simple as that:

```cpp
Find(key) {
    Link current = 0;
    for (int b = 0; b < KEY_BIT_LEN; ++b) {
      if (key differs from nodes[current].key in the b-th bit) {
        current = nodes[current].link[bit];
      }
    }
    return current;
}
```

Time complexity: <img src="docs/O(KeyBitLen).png" alt="alt text" height=20>.

### Insert

Is a bit harder. When we adding a new key we also need to correctly update the links of those preceiding nodes for which the new node is "the nearest node (from the right) with a key having exactly the same first...".

But there are only
<img src="docs/O(NKeyBitLen).png" alt="alt text" height=20>
links in total. It turns out that we can guarantee that each link will be assigned only once (and few other technicalities). In short, we can do `Insert` in amortized
<img src="docs/O(KeyBitLen).png" alt="alt text" height=20>
time.<br/>
Please refer to the implementation if you are interested in details.

### Delete (the most recently inserted)

TODO. I'm pretty confident that we are able to do it with the same amortized complexity as `Insert`.

## Possible applications



