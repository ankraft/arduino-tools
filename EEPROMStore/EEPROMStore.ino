/*
 *	EEPROMStore.ino
 *
 *	copyright (c) Andreas Kraft 2018
 *	Licensed under the BSD 3-Clause License. See the LICENSE file for further details.
 *
 * A support class to persistently store values in the processors EEPROM.
 */

# include "EEPROMStore.h"
# include <EEPROM.h>


EEPROMStore::EEPROMStore(const int maxNumberOfEntries, const int entrySize) {
	_init(maxNumberOfEntries + 1, entrySize, 0); // + store identifier
}


EEPROMStore::EEPROMStore(const int maxNumberOfEntries, const int entrySize, const int startAddress) {
	_init(maxNumberOfEntries + 1, entrySize, startAddress); // + store identifier
}

void EEPROMStore::_init(const int maxNumberOfEntries, const int entrySize, const int startAddress) {
	this->maxNumberOfEntries = maxNumberOfEntries; 
	this->entrySize = entrySize;
	this->startAddress = startAddress;
	this->totalSize = this->maxNumberOfEntries * this->entrySize;
	this->ready = false;

# if defined(ESP8266) || defined(ESP32)
	if (totalSize < 4 || totalSize > 4096) {
		return;
	}
	EEPROM.begin(this->totalSize);
# endif

	this->ready = true;
}


EEPROMStore::~EEPROMStore() {
# if defined(ESP8266) || defined(ESP32)
	EEPROM.end();
# endif
	this->ready = false;
}


String EEPROMStore::getString(const unsigned int index) {
	if (index < 0 || index >= maxNumberOfEntries) {
		return "";
	}
	return this->_getString(index + 1);
}


String EEPROMStore::_getString(const unsigned int index) {
	int offset = startAddress + (index * entrySize);
	char *str = new char[entrySize];
	for (int i = 0; i < entrySize; i++) {
		str[i] = EEPROM.read(offset + i);
	}
	return String(str);
}


void EEPROMStore::putString(const unsigned int index, const String value) {
	if (index < 0 || index >= maxNumberOfEntries) {
		return ;
	}
	this->_putString(index + 1, value);
}


void EEPROMStore::_putString(const unsigned int index, const String value) {
	const char * str = value.c_str();
	int len = strlen(str);
	int offset = startAddress + (index * entrySize);
	for (int i = 0; i < len; i++) {
		EEPROM.write(offset + i, str[i]);
	}
	EEPROM.write(offset + len, '\0');

# if defined(ESP8266) || defined(ESP32)
	EEPROM.commit();
# endif
}


template<typename T> 
T &EEPROMStore::get(const unsigned int index, T &t) {
	if (index < 0 || index >= maxNumberOfEntries) {
		return t;
	}
	return this->_get(index + 1, t);
}


template<typename T> 
T &EEPROMStore::_get(const unsigned int index, T &t) {
	return EEPROM.get(startAddress + (index * entrySize), t);
}


template<typename T> 
void EEPROMStore::put(const unsigned int index, const T &t) {
	if (index < 0 || index >= maxNumberOfEntries) {
		return;
	}
	this->_put(index + 1, t);
}


template<typename T> 
void EEPROMStore::_put(const unsigned int index, const T &t) {
	EEPROM.put(startAddress + (index * entrySize), t);

# if defined(ESP8266) || defined(ESP32)
	EEPROM.commit();
# endif
}


void EEPROMStore::clear() {
	for (int i = startAddress; i < (startAddress + totalSize); i++) {
		EEPROM.write(i, 0);
	}
}


void EEPROMStore::setStoreIdentifier(const String storeIdentifier) {
	this->_putString(0, storeIdentifier);
}


String EEPROMStore::getStoreIdentifier() {
	return this->_getString(0);
}
