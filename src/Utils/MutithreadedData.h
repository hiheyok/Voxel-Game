#pragma once
#define _CRTDBG_MAP_ALLOC
#include <deque>
#include <unordered_map>
#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>
#include <concurrent_priority_queue.h>
#include <concurrent_vector.h>
#include <concurrent_queue.h>

#include <vector>
#include <list>
#include <mutex>

template <typename Hasher, typename Object> class AsyncHashMap {
public:concurrency::concurrent_unordered_map<Hasher, Object> HashMap;

	  bool count(Hasher Hash) {
		  return HashMap.count(Hash);
	  }

	  bool empty() {
		  return HashMap.empty();
	  }

	  void erase(Hasher Hash) {
		  Mut.lock();
		  HashMap.unsafe_erase(Hash);
		  Mut.unlock();
	  }

	  void clear() {
		  HashMap.clear();
		  MEM_USAGE == 0;
	  }

	  size_t size() {
		  return HashMap.size();
	  }

	  concurrency::concurrent_unordered_map<Hasher, Object> DumpData() {
		  return HashMap;
	  }

	  Object& operator[](Hasher Hash) {
		  return HashMap[Hash];
	  }

private:
	std::mutex Mut;
	size_t MEM_USAGE = 0;
};


template <typename Object> class AsyncDeque {
public:std::deque<Object> Deque;
	  size_t size() {
		  size_t size = 0;

		  Mut.lock();
		  size = Deque.size();
		  Mut.unlock();
		  return size;
	  }

	  bool empty() {
		  bool IsEmpty = false;
		  Mut.lock();
		  IsEmpty = Deque.empty();
		  Mut.unlock();
		  return IsEmpty;
	  }

	  void push_back(Object Obj) {
		  Mut.lock();
		  Deque.push_back(Obj);
		  Mut.unlock();
	  }

	  void push_front(Object Obj) {
		  Mut.lock();
		  Deque.push_front(Obj);
		  Mut.unlock();
	  }

	  void pop_back() {
		  Mut.lock();
		  Deque.push_back();
		  Mut.unlock();
	  }

	  void pop_front() {
		  Mut.lock();
		  Deque.pop_front();
		  Mut.unlock();
	  }

	  void clear() {
		  Mut.lock();
		  Deque.clear();
		  Mut.unlock();
	  }

	  void emplace_back(Object Obj) {
		  Mut.lock();
		  Deque.emplace_back(Obj);
		  Mut.unlock();
	  }

	  void emplace_front(Object Obj) {
		  Mut.lock();
		  Deque.emplace_front(Obj);
		  Mut.unlock();
	  }

	  Object front() {
		  Object Obj;
		  Mut.lock();
		  Obj = Deque.front();
		  Mut.unlock();
		  return Obj;
	  }

	  Object pop_get_front() {
		  Object Obj;
		  Mut.lock();
		  Obj = Deque.front();
		  Deque.pop_front();
		  Mut.unlock();
		  return Obj;
	  }

	  Object back() {
		  Object Obj;
		  Mut.lock();
		  Obj = Deque.back();
		  Mut.unlock();
		  return Obj;
	  }
	  Object pop_get_back() {
		  Object Obj;
		  Mut.lock();
		  Obj = Deque.back();
		  Deque.pop_back();
		  Mut.unlock();
		  return Obj;
	  }

private:
	std::mutex Mut;
};

template <typename Object> class AsyncVector {
public:std::vector<Object> Vector;
	size_t size() {
		size_t size = 0;

		Mut.lock();
		size = Vector.size();
		Mut.unlock();
		return size;
	}

	bool empty() {
		bool IsEmpty = false;
		Mut.lock();
		IsEmpty = Vector.empty();
		Mut.unlock();
		return IsEmpty;
	}

	void push_back(Object Obj) {
		Mut.lock();
		Vector.push_back(Obj);
		Mut.unlock();
	}

	void pop_back() {
		Mut.lock();
		Vector.push_back();
		Mut.unlock();
	}

	void clear() {
		Mut.lock();
		Vector.clear();
		Mut.unlock();
	}

	void emplace_back(Object Obj) {
		Mut.lock();
		Vector.emplace_back(Obj);
		Mut.unlock();
	}

	void emplace_front(Object Obj) {
		Mut.lock();
		Vector.emplace_front(Obj);
		Mut.unlock();
	}

	Object front() {
		Object Obj;
		Mut.lock();
		Obj = Vector.front();
		Mut.unlock();
		return Obj;
	}

	Object back() {
		Object Obj;
		Mut.lock();
		Obj = Vector.back();
		Mut.unlock();
		return Obj;
	}

private:
	std::mutex Mut;
};
