# LinkedList

This template class provides a single-linked list implementation.

## Installation

Just copy the files from this directory to your project.

## Usage

The following example creates a linked list for ```String``` objects.

```cpp
# include "LinkedList.h"

LinkedList<String>	stringList;
```

## Class methods

- **int size()**  
Return the number of items in the list.
- **bool add(T object, int position)**  
Add an *object* at *position* in the list. This method returns false if *position* points to an invalid index, true otherwise.
- **bool add(T)**  
Append an *object* to the end of the list. This method returns false if the *object* couldn't be appended, true otherwise.
- **T get(int position)**  
Return the object at position *position*. This method returns a new object of class *T* in case of an error.
- **T first()**  
Return the first object from the list. This method returns a new object of class *T* in case of an error.
- **T last()**  
Return the first object from the list. This method returns a new object of class *T* in case of an error.
- **bool remove()**  
Remove the last object from the list. This method returns true if successful, or false otherwise.
- **bool remove(int position)**  
Remove the object at position *position*. The object is deleted. This method returns true if successful, or false otherwise.
- **void clear()**  
Clear the list of all objects. The objects are deleted.


## License
 Licensed under the BSD 3-Clause License. See the [LICENSE](../LICENSE) file for further details.
