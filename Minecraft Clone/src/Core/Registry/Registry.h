#pragma once
#include <unordered_map>
#include <string>
#include <exception>
#include "../../Level/Typenames.h"

template <class K, class V> class Registry {
public:
	V GetValue(K key) {
		if (!Hashmap.count(key)) {
			throw std::exception("Cannot return value because key doesn't exist");
		}
		return Hashmap[key];
	}

	K getKey(int ID) {
		if (!getKeybyID.count(ID)) {
			throw std::exception("Cannot return key because ID doesn't exist");
		}
		return getKeybyID[ID];
	}

	int getID(K key) {
		if (!getIDbyKey.count(key)) {
			throw std::exception("Cannot return ID because key doesn't exist");
		}
		return getIDbyKey[key];
	}

	void Register(int ID, K key, V value) {
		Hashmap.emplace(std::pair<K, V>(key, value));
		getIDbyKey.emplace(std::pair<K, int>(key, ID));
		getKeybyID.emplace(std::pair<int, K>(ID, key));
	}

private:
	FastHashMap<K, V> Hashmap;
	FastHashMap<K, int> getIDbyKey;
	FastHashMap<int, K> getKeybyID;
};