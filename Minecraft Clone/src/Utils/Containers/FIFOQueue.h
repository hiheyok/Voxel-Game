#pragma once
template <class T>
class FixedFIFOQueue {
private:
	T* mBuffer = NULL;
	bool mIsInitialized = false;

	uint64_t startPtr = 0;
	uint64_t endPtr = 0;
	uint64_t mQueueSize = 0;
	uint64_t mElementCount = 0;
public:
	FixedFIFOQueue() {

	}

	FixedFIFOQueue(uint64_t size) {
		setSize(size);
	}

	void setSize(uint64_t pSize) {
		if (mIsInitialized) delete[] mBuffer;
		mQueueSize = pSize;
		mBuffer = new T[mQueueSize];
		mIsInitialized = true;
	}

	inline void push(const T& obj) {
		mBuffer[endPtr++] = obj;
		endPtr = endPtr % mQueueSize;
		++mElementCount;
	}

	inline T get() {
		T out = mBuffer[startPtr++];
		startPtr = startPtr % mQueueSize;
		--mElementCount;
		return out;
	}

	void resetData() {
		startPtr = 0;
		endPtr = 0;
		mElementCount = 0;
	}

	void clear() {
		if (mIsInitialized) delete[] mBuffer;
		startPtr = 0;
		endPtr = 0;
		mQueueSize = 0;
		mElementCount = 0;
		mIsInitialized = false;
	}

	

	inline bool isEmpty() {
		return mElementCount <= 0;
	}

	inline bool isFull() {
		return mElementCount >= mQueueSize;
	}

	
};