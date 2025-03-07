#pragma once
template <class T>
class FixedFIFOQueue {
public:
    FixedFIFOQueue();
    FixedFIFOQueue(uint64_t size);
    ~FixedFIFOQueue();

    FixedFIFOQueue(FixedFIFOQueue&& q) = delete;
    FixedFIFOQueue(const FixedFIFOQueue& q) = delete;

    void setSize(uint64_t size);
    void push(const T& obj);

    T get();
    void resetData();
    void clear();
    bool IsEmpty() const;
    bool IsFull() const;
    bool IsInitialized() const;
private:
    T* buffer_ = nullptr;
    bool is_initialized_ = false;

    uint64_t start_ptr_ = 0;
    uint64_t end_ptr_ = 0;
    uint64_t queue_size_ = 0;
    uint64_t element_count_ = 0;
};

template <class T>
inline FixedFIFOQueue<T>::FixedFIFOQueue() {}

template <class T>
inline FixedFIFOQueue<T>::~FixedFIFOQueue() {
    if (buffer_ != nullptr) delete[] buffer_;
}

template <class T>
inline FixedFIFOQueue<T>::FixedFIFOQueue(uint64_t size) {
    setSize(size); 
}

template <class T>
inline void FixedFIFOQueue<T>::setSize(uint64_t size) {
    if (is_initialized_) delete[] buffer_;
    queue_size_ = size;
    buffer_ = new T[queue_size_];
    is_initialized_ = true;
}

template <class T>
inline void FixedFIFOQueue<T>::push(const T& obj) {
    buffer_[end_ptr_++] = obj;
    end_ptr_ = end_ptr_ % queue_size_;
    ++element_count_;
}

template <class T>
inline T FixedFIFOQueue<T>::get() {
    T out = buffer_[start_ptr_++];
    start_ptr_ = start_ptr_ % queue_size_;
    --element_count_;
    return out;
}

template <class T>
inline void FixedFIFOQueue<T>::resetData() {
    start_ptr_ = 0;
    end_ptr_ = 0;
    element_count_ = 0;
}

template <class T>
inline void FixedFIFOQueue<T>::clear() {
    if (is_initialized_) delete[] buffer_;
    start_ptr_ = 0;
    end_ptr_ = 0;
    queue_size_ = 0;
    element_count_ = 0;
    is_initialized_ = false;
}

template <class T>
inline bool FixedFIFOQueue<T>::IsEmpty() const {
    return element_count_ <= 0;
}

template <class T>
inline bool FixedFIFOQueue<T>::IsFull() const {
    return element_count_ >= queue_size_;
}

template <class T>
inline bool FixedFIFOQueue<T>::IsInitialized() const {
    return is_initialized_;
}