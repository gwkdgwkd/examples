#ifndef OPENGLES_UTILS_THREADSAFEQUEUE_H
#define OPENGLES_UTILS_THREADSAFEQUEUE_H

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {

public:
    ThreadSafeQueue() {}

    ThreadSafeQueue(ThreadSafeQueue const &other) {
        std::lock_guard <std::mutex> lk(other.m_mutex);
        data_queue_ = other.m_dataQueue;
    }

    void Push(T new_value) {
        std::lock_guard <std::mutex> lk(mutex_);
        data_queue_.push(new_value);
        condition_.notify_one();
    }

    T Pop() {
        std::unique_lock <std::mutex> lk(mutex_);
        if (Empty()) return nullptr;
        T res = data_queue_.front();
        data_queue_.pop();
        return res;
    }

    bool Empty() const {
        return data_queue_.empty();
    }

    int Size() {
        std::unique_lock <std::mutex> lk(mutex_);
        return data_queue_.size();
    }

private:
    mutable std::mutex mutex_;
    std::queue <T> data_queue_;
    std::condition_variable condition_;
};

#endif // OPENGLES_UTILS_THREADSAFEQUEUE_H
