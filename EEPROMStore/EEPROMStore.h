/*
 *	EEPROMStore.h
 *
 *	copyright (c) Andreas Kraft 2018
 *	Licensed under the BSD 3-Clause License. See the LICENSE file for further details.
 *
 *	A support class to persistently store values in the processors EEPROM.
 */

# ifndef __EEPROMSTORE__
# define __EEPROMSTORE__


class EEPROMStore {

private:
	bool 	ready;							// internal ready flag

private:
	unsigned int 	maxNumberOfEntries;		// number of entires. This is +1 for the store identifier
	unsigned int 	entrySize;				// reserved size for each entry
	unsigned int 	startAddress;			// start address in the EEPROM, in case one wants to hold multiple stores
	unsigned int 	totalSize;				// total size of this store

	// internally used methods
	void 	_init(const int maxNumberOfEntries, const int entrySize, const int startAddress);
	String 	_getString(const unsigned int index);
	void 	_putString(const unsigned int index, const String value);

	template<typename T> 
  	T &_get(const unsigned int index, T &t);

	template<typename T> 
	void _put(const unsigned int index, const T &t);

public:

	// Constructor to initialize the EEPROM store.
	// *maxNumberOfEntries* specifies the maximum number of entries for this store.
	// *entrySize* specifies the maximum size for each entry.	
	EEPROMStore(const int maxNumberOfEntries, const int entrySize);

	// Constructor to initialize the EEPROM store.
	// *maxNumberOfEntries* specifies the maximum number of entries for this store.
	// *entrySize* specifies the maximum size for each entry.
	// *startAddress* specifies the memory address in the store where the store starts to store the data. The default is 0.
	EEPROMStore(const int maxNumberOfEntries, const int entrySize, const int startAddress);

	// Destructor
	~EEPROMStore();

	// Retrieve a String value from the store.
	// *index* is the entry index in the store.
	// If *index* is invalid, an empty String is returned.
	String 	getString(const unsigned int index);

	// Store a String value in the store.
	// *index* is the entry index in the store.
	// *value* is the String to store.
	void 	putString(const unsigned int index, const String value);

	// Retrieve a common scalar type from the store.
	// *index* is the entry index in the store.
	// *t* is a variable of the type to retrieve. This is only used to determine the type and space needed
	// If *index* is invalid, *t* is returned.
	template<typename T> 
  	T &get(const unsigned int index, T &t);

  	// Store a common scalar type in the store.
	// *index* is the entry index in the store.
	// *t* is a variable of the type to store. This is only used to determine the type and space needed.
	template<typename T> 
	void put(const unsigned int index, const T &t);

	// Retrieve the store identifier. This identifier is stored as the first entry in the store and must
	// not exceed the entry size as specified in the constructor. It can be used to identify the store's content.
	String getStoreIdentifier();

	// Set an identifier for the store. This identifier is stored as the first entry in the store and must
	// not exceed the entry size as specified in the constructor. It can be used to identify the store's content.
	// *storeIdentifier* specifies the store identifier.
	void setStoreIdentifier(const String storeIdentifier);


	// Clear the store. This is done by writing 0 values to the EEPROM in the memory cells reserved for the store.
	void clear();

};

# endif
