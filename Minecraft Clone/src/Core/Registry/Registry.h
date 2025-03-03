#pragma once
#include <unordered_map>
#include <string>
#include <exception>
#include "../../Level/Typenames.h"

template <class K, class V> class Registry {
public:
	V GetValue(K key) {
		if (!hashmap_.count(key)) {
			throw std::exception("Cannot return value because key doesn't exist");
		}
		return hashmap_[key];
	}

	K GetKey(int id) {
		if (!get_key_by_id_.count(id)) {
			throw std::exception("Cannot return key because ID doesn't exist");
		}
		return get_key_by_id_[id];
	}

	int GetId(K key) {
		if (!get_id_by_key_.count(key)) {
			throw std::exception("Cannot return ID because key doesn't exist");
		}
		return get_id_by_key_[key];
	}

	void Register(int id, K key, V value) {
		hashmap_.emplace(key, value);
		get_id_by_key_.emplace(key, id);
		get_key_by_id_.emplace(id, key);
	}

private:
	FastHashMap<K, V> hashmap_;
	FastHashMap<K, int> get_id_by_key_;
	FastHashMap<int, K> get_key_by_id_;
};