#pragma once
template <class T>
class FixedFIFOQueue {
private:
	T* buffer_ = NULL;
	bool is_initialized_ = false;

	uint64_t start_ptr_ = 0;
	uint64_t end_ptr_ = 0;
	uint64_t queue_size_ = 0;
	uint64_t element_count_ = 0;
public:
	FixedFIFOQueue() {

	}

	FixedFIFOQueue(uint64_t size) {
		setSize(size);
	}

	void setSize(uint64_t size) {
		if (is_initialized_) delete[] buffer_;
		queue_size_ = size;
		buffer_ = new T[queue_size_];
		is_initialized_ = true;
	}

	inline void push(const T& obj) {
		buffer_[end_ptr_++] = obj;
		end_ptr_ = end_ptr_ % queue_size_;
		++element_count_;
	}

	inline T get() {
		T out = buffer_[start_ptr_++];
		start_ptr_ = start_ptr_ % queue_size_;
		--element_count_;
		return out;
	}

	void resetData() {
		start_ptr_ = 0;
		end_ptr_ = 0;
		element_count_ = 0;
	}

	void clear() {
		if (is_initialized_) delete[] buffer_;
		start_ptr_ = 0;
		end_ptr_ = 0;
		queue_size_ = 0;
		element_count_ = 0;
		is_initialized_ = false;
	}

	

	inline bool IsEmpty() {
		return element_count_ <= 0;
	}

	inline bool IsFull() {
		return element_count_ >= queue_size_;
	}

	
};