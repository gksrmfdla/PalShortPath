#pragma once
#include <cassert>
// circular queue with succesive memory
template <typename T>
class Queue {
public:
    Queue() : mCapacity(4), mStartIdx(0), mEndIdx(0), mLength(0) {
        mArr = new T[mCapacity];
    }
    ~Queue() {
        delete[] mArr;
        mArr = nullptr;
    }

    void Clear() {
        mStartIdx = 0;
        mEndIdx = 0;
        mLength = 0;
    }

    void Push(T value) {
        // increase capacity
        if (mLength == mCapacity) {
            int newCapacity = mCapacity * 2;
            T* newArr = new T[newCapacity];
            if (mLength == (mEndIdx - mStartIdx + 1)) {
                memcpy(newArr, mArr + mStartIdx, sizeof(T) * mLength);
            }
            else {
                uint32_t firstSplitLength = mCapacity - mStartIdx;
                memcpy(newArr, mArr + mStartIdx, sizeof(T) * (firstSplitLength));
                memcpy(newArr + firstSplitLength, mArr, sizeof(T) * (mEndIdx + 1));
            }

            mStartIdx = 0;
            mEndIdx = mLength - 1;
            mCapacity = newCapacity;

            delete[] mArr;
            mArr = newArr;
        }

        if (mLength == 0) {
            mStartIdx = 0;
            mEndIdx = 0;
        }
        else {
            mEndIdx = (mEndIdx + 1) % mCapacity;
        }

        mArr[mEndIdx] = value;
        mLength++;
    }

    T Pop() {
        assert(mLength != 0);

        int deletedIdx = mStartIdx;
        mStartIdx = (mStartIdx + 1) % mCapacity;
        mLength--;

        if (mLength == 0) {
            mStartIdx = 0;
            mEndIdx = 0;
        }

        return mArr[deletedIdx];
    }

    T Front() {
        assert(mLength != 0);

        return mArr[mStartIdx];
    }

    T Last() {
        assert(mLength != 0);

        return mArr[mEndIdx];
    }

    inline uint32_t GetLength() {
        return mLength;
    }


private:
    int mStartIdx;
    int mEndIdx;
    T* mArr = nullptr;
    uint32_t mCapacity = 0;
    uint32_t mLength = 0;
};
