# flov
*Dunno whether the following idea is well known or not. But I never heard it before.*

## The problem
We have a sequence of integers, which grows time after time. And sometimes we want to check, if the given integer is already present in our sequence or not. So, we need a data structure with operations `Insert(key)` and `Find(key)`.

## The approach
Lets store all the numbers in a vector-like manner: new key is always placed right after the last one.
And for every node maintained a table `link[]` the size of our integer type (in bits).

So, `node.link[5]` will be the link to the nearest node (from the right) with a key having exactly the same first 5 bits (from 0-th to 4-th) and which fifth bit is the opposite to the `node.key`'s fifth bit.

## Implementation

### Find

Find became as simple as that:

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

Time complexity: *O(KEY_BIT_LEN)*.

### Insert

Is a bit harder. When we adding a new key we also need to correctly update all the `link`-s of the preceiding keys for which new key is "the nearest node (from the right) with a key having exactly the same first...". There can be many of such nodes. For example, if all currently added keys were from `[0, 15]` then adding `16` will result in updating of the `link[4]` value of all nodes. It means that we cannot guarantee any complexity better than *O(N)* for each `PushBack` call. But there only *O(N***KEY_BIT_LEN)* links in total. It turns out that we can guarantee that each link will be assigned only once (and few other technicalities). In short, we can do `PushBack` in amortized *O(KEY_BIT_LEN)* time. Please refer to the implementation if you interested in details.

### Delete (the most recently inserted)

TODO. I'm pretty confident that we are able to do it with the same amortized complexity as `PushBack`.

#### Further improvements

Reduce memory overhead? Currently, space complexity is *O(N***KEY_BIT_LEN)*.
