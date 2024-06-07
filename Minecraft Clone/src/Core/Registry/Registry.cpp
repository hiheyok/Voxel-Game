#include "Registry.h"


template <class K, class V> V Registry<K, V>::GetValue(K key) {
	if (!Hashmap.count(key)) {
		throw std::exception("Cannot return value because key doesn't exist");
	}
	return Hashmap[key];
}

template <class K, class V> K Registry<K, V>::getKey(int ID) {
	if (!getKeybyID.count(ID)) {
		throw std::exception("Cannot return key because ID doesn't exist");
	}
	return getKeybyID[ID];
}

template <class K, class V> int Registry<K, V>::getID(K key) {
	if (!getIDbyKey.count(key)) {
		throw std::exception("Cannot return ID because key doesn't exist");
	}
	return getIDbyKey[key];
}

template <class K, class V> void Registry<K, V>::Register(int ID, K key, V value) {
	Hashmap.insert(std::pair<K, V>(key, value));
	getIDbyKey.insert(std::pair<K, int>(key, ID));
	getKeybyID.insert(std::pair<int, K>(ID, key));
}