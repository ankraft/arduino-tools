# RingBuffer

This template class provides a ring buffer implementation.

## Installation

Just copy the files from this directory to your project.

## Usage

The following example creates a ring buffer for up to 20 ```String``` objects. If more than 20 objects are added to the ring buffer, then the oldest object is removed from the ring buffer.

```cpp
# include "RingBuffer.h"

RingBuffer<String>	ringBuffer(20);
```

**Note**: This template class doesn't do memory management on the stored objects.

## Class methods
- **void add(T item)**  
Add an *item* to the end of the ring buffer. If the buffer is full, then the oldest item in the buffer is overwritten.
- **T get(int index)**  
Get an item from the ring buffer. *index* is relative to the beginning of the buffer, ie ```get(0)``` returns the oldest item while ```get(count())``` returns the newest item from the buffer.  
If *index* is invalid, a new object of Type T is returned.
- **T getReverse(int index)**  
Get an item from the ring buffer. This method acts exactly in reverse to the *get()* method, ie ```get(0)``` returns the newest item while ```get(count())``` returns the oldest item from the buffer.  
If *index* is invalid, a new object of Type T is returned.
- **T getOldest()**  
Get the oldest item from the ring buffer.  
If the buffer is empty, a new object of Type T is returned.
- **T getLatest()**  
Get the latest item from the ring buffer.  
If the buffer is empty, a new object of Type T is returned.
- **bool slice(int count)**  
Remove *count* elements from the tail and head of the ring buffer. The size of the ring buffer is reduced by *count* * 2 items.  
The method returns true if successful, false otherwise.
- **bool sliceHead(int count)**  
Remove *count* elements from the head of the ring buffer. The size of the ring buffer is reduced by *count* items.  
The method returns true if successful, false otherwise.
- **bool sliceTail(int count)**  
Remove *count* elements from the tail of the ring buffer. The size of the ring buffer is reduced by *count* items.  
The method returns true if successful, false otherwise.
- **void clear()**  
Empty the ring buffer. Objects in the ring buffer are **not** deleted from memory.
- **int size()**  
Return the size of the ring buffer.
- **int count()**  
Return the number of elements in the ring buffer.
- **bool isEmpty()**  
Check whether the ring buffer is empty.
- **bool isFull()**  
Check whether the ring buffer is full.
- **T operator[](int index)**  
Using this assignment operator is equivalent to calling the *get()* method.
- **RingBuffer&lt;T> &operator=(T item)**  
Using this assignment operator is equivalent to calling the *add()* method.

## License
 Licensed under the BSD 3-Clause License. See the [LICENSE](../LICENSE) file for further details.
