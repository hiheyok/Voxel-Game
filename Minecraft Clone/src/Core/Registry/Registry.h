#pragma once
#include <unordered_map>
#include <string>
#include <exception>

template <class K, class V> class Registry {
public:
	V GetValue(K key);

	K getKey(int ID);

	int getID(K key);

	void Register(int ID, K key, V value);

private:
	std::unordered_map<K, V> Hashmap;
	std::unordered_map<K, int> getIDbyKey;
	std::unordered_map<int, K> getKeybyID;
};