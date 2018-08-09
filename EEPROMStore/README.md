# EEPROMStore

A support class to persistently store values in a processor's EEPROM.

## Installation

Just copy the files from this directory to your project.

## Usage

The following example creates a store for 5 entries and 32 bytes for each entry.

```cpp
# include "EEPROMStore.h"

EEPROMStore store = EEPROMStore(5, 32);

```

### Store a String Value

The following example stores the string "Hello, World" at index 0 in the store.

```cpp
store.putString(0, "Hello, World");
```

### Retrieve a String Value

The following example retrieves the string value stored at index 0.

```cpp
String str = store.getString(0);
```

### Working with Store Identifiers

In order to make sure that a store is initialized and contains previous stored valid entries, or that it is the correct store at all, an identifier can be assigned to the store. That identifier is stored as the beginning of the store.

The identifier must be a string and conform to the maximum length for entries of the store.

The following example shows how to check for the correct store and, if not found, initializes the store.

```cpp
if (store.getStoreIdentifier() != "MYSTORE") {

  // clear the store and set the store identifier
  store.clear();
  store.setStoreIdentifier("MYSTORE");

  // set default values
  store.setString(0, "first value");
  store.setString(1, "second value");
}
```

## Class Definition

The *EEPROMStore* class defines the following constructors, types and structures.

### Constructors

- **EEPROMStore(const int maxNumberOfEntries, const int entrySize)**  
Constructor to initialize the EEPROM store.  
*maxNumberOfEntries* specifies the maximum number of entries for this store.  
*entrySize* specifies the maximum size for each entry.
- **EEPROMStore(const int maxNumberOfEntries, const int entrySize, const int startAddress)**  
Constructor to initialize the EEPROM store.  
*maxNumberOfEntries* specifies the maximum number of entries for this store.  
*entrySize* specifies the maximum size for each entry.  
*startAddress* specifies the memory address in the store where the store starts to store the data. The default is 0.

### Class Methods

- **String getString(const unsigned int index)**  
Retrieve a String value from the store.  
*index* is the entry index in the store.  
If *index* is invalid, an empty String is returned.
- **void putString(const unsigned int index, const String value)**  
Store a String value in the store.  
*index* is the entry index in the store.  
*value* is the String to store.
- **template&lt;typename T>**  
**T &get(const unsigned int index, T &t)**  
Retrieve a common scalar type from the store.  
*index* is the entry index in the store.  
*t* is a variable of the type to retrieve. This is only used to determine the type and space needed.
If *index* is invalid, *t* is returned.
- **template&lt;typename T>**  
**void put(const unsigned int index, const T &t)**  
Store a common scalar type in the store.  
*index* is the entry index in the store.  
*t* is a variable of the type to store. This is only used to determine the type and space needed.
- **String getStoreIdentifier()**  
Retrieve the store identifier. This identifier is stored as the first entry in the store and must not exceed the entry size as specified in the constructor. It can be used to identify the store's content.
- **void setStoreIdentifier(const String storeIdentifier)**  
Set an identifier for the store. This identifier is stored as the first entry in the store and must not exceed the entry size as specified in the constructor. It can be used to identify the store's content.  
*storeIdentifier* specifies the store identifier.  
- **void clear()**  
Clear the store. This is done by writing 0 values to the EEPROM in the memory cells reserved for the store.

## Compatibility

This support class has been tested with ESP8266 (Wemos D1) and ESP32 boards.
