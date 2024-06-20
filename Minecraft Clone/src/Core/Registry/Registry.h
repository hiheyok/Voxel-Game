#pragma once
#include <unordered_map>
#include <string>
#include <exception>

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
	std::unordered_map<K, V> Hashmap;
	std::unordered_map<K, int> getIDbyKey;
	std::unordered_map<int, K> getKeybyID;
};