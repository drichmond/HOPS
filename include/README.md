# HOPS library

This folder contains our library implementation. At present it contains the
following files and functions:

**Note:** The functions are described below have function and class-wrapped
variants. The function are named using lower-case. The class-wrapped variants
are named with the first letter capitalized (e.g. Map).

## Higher-Order Function Library Files

These files implement Higher-Order Functions. Higher-Order Functions take at
least one argument as an input.

### functools.hpp

These functions are useful for manipulating functions to get an appropriate type
signature.

| Function      | Description |
| ------------- |:----------- |
| [**compose**](http://hackage.haskell.org/package/transformers-0.5.1.0/docs/Data-Functor-Compose.html#v:Compose) | Given two functions, return a new function with the second function as the input to the first (right to left) |
| **partial** | Given two functions, use the second function as the first argument to the first function.
| [**curry**](http://hackage.haskell.org/package/transformers-0.5.1.0/docs/Data-Functor-Compose.html#v:Compose) | Given a single-input function on a pair, return a new input-function (Haskell sez: Converts an uncurried function into a curried function. |
| [**uncurry**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Prelude.html#v:uncurry) | Given a two-input function convert it into a function on pairs. (Haskell sez: Converts a curried function into an uncurried one) |
| [**flip**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Prelude.html#v:flip) | Given a two-input function, flip the order of its arguments |

### map.hpp

The map function is useful for applying a function to each element in an array.

| Function      | Description |
| ------------- |:----------- |
| [**map**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Prelude.html#v:map) | Given an array, apply a function to each element and return the result as an array |

### **reduce.hpp**

The reduce functions are useful for computing an aggregate result of an array, and
some transformations (such as reverse).

| Function      | Description |
| ------------- |:----------- |
| [**rreduce**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Prelude.html#v:foldr) | Given an array, iterate from right to left using a binary operator and a starting value.|
| [**lreduce**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Prelude.html#v:foldl) | Given an array, iterate from left to right using a binary operator and a starting value.|


### **divconq.hpp**

The Divide-And-Conquer function is useful for specifying tree-like structures
and can be used to permute arrays, compute aggregate results, or implement
larger multi-stage recursive algorithms.

| Function      | Description |
| ------------- |:----------- |
| **divconq**   | Given an input array recurisvely split an array in halves until single-element arrays when the base case of 1-element arrays is reached and apply a function to adjacent pairs |


### **zip.hpp**

The zip functions are useful for pairing arrays together to apply functions and
manipulate arrays. Only the **zipWith** function is a Higher-Order function.

| Function      | Description |
| ------------- |:----------- |
| [**zipWith**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Prelude.html#v:zipWith) | Given two arrays apply a function to corresponding in each and return an array of results. |

## Array Manipulation Files

These functions in these files manipulate arrays but are not Higher-Order
Functions.

### **zip.hpp**

The zip functions are useful for pairing arrays together to manipulate
arrays. **zip** and **unzip** are not Higher-Order functions.

| Function      | Description |
| ------------- |:----------- |
|[**zip**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Prelude.html#v:zip) | Given two arrays return an array of corresponding pairs |
|[**unzip**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Prelude.html#v:zip) | Given an array of pairs return an array of first components and an array of second components as a pair. |

### **arrayops.hpp**

The arrayops functions are useful for simple manipulations of arrays.

| Function        | Description |
| --------------- |:----------- |
| **log2**       | Given a static value, return the log-base-2 of a value (returns 1 when input is 0) | 
| **clog2**      | Given a static value, return the ceiling-log-base-2 of a value (returns 1 when input is 0) | 
| [**range**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Data-Ix.html#v:range) | Given compile-time template parameters STOP, START, and STEP return an array with length equal to (STOP-START)/STEP, first element equal to START, and subsequent values spaced by STEP |
| [**reverse**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Prelude.html#v:reverse) | Given an array, return a new array with the elements in reverse order |
| **prepend** | Given a value and an N-element array, return a N+1-element array with the new value at the first index |
| **append**      | Given an N-element array and a value, return a N+1-element array the new value at the last index |
| **concatenate** | Given an N-element array L, and an M-element array R, return an N+M-element array where values from L preceed the values from R |
| [**replicate**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Prelude.html#v:replicate) | Given a value, and length N as a compile-time template parameter return an N-element array filled with the value |
| [**split**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Prelude.html#v:splitAt) | Given an input array, and a compile-time index N, split the array at index N and return the sub-arrays as a pair |
| [**head**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Prelude.html#v:head) | Given an array return the first element |
| [**tail**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Prelude.html#v:tail) | Given an N-element array, return N-1 element array with all non-head elements |
| [**last**](http://hackage.haskell.org/package/base-4.11.1.0/docs/Prelude.html#v:last) | Given an array return the last element |
| **rrotate**     | Given an array return a new array with the last element moved to the front |
| **rshift**      | Given an array and a value, return an array with the the value at the first element, and remove the last element | |
| **lrotate**     | Given an array return an array with the first element moved to the end of the array |
| **lshift**      | Given an array and a value, return an array with the last element of the input array removed and the new value at the last index |
| **construct**   | Given two values, return a new, two-element array (Same as **array**) |


