#include "thread_base.h"

#include <iostream>
#include <log.h>

ThreadBase::ThreadBase()
        : thread_(nullptr),
          pause_flag_(false),
          stop_flag_(false),
          state_(STOPED) {
    TRACE_FUNC();
}

ThreadBase::~ThreadBase() {
    TRACE_FUNC();
    Stop();
}

enum ThreadBase::State ThreadBase::State() const {
    TRACE_FUNC();
    return state_;
}

void ThreadBase::Start() {
    TRACE_FUNC();
    if (thread_ == nullptr) {
        thread_ = new std::thread(&ThreadBase::Run, this);
        pause_flag_ = false;
        stop_flag_ = false;
        state_ = RUNNING;
    } else {
        Resume();
    }
}

void ThreadBase::Stop() {
    TRACE_FUNC();
    if (thread_ != nullptr) {
        pause_flag_ = false;
        stop_flag_ = true;
        condition_.notify_all();
        thread_->join();
        delete thread_;
        thread_ = nullptr;
        state_ = STOPED;
    }
}

void ThreadBase::Pause() {
    TRACE_FUNC();
    if (thread_ != nullptr) {
        pause_flag_ = true;
        state_ = PAUSED;
    }
}

void ThreadBase::Resume() {
    TRACE_FUNC();
    if (thread_ != nullptr) {
        pause_flag_ = false;
        condition_.notify_all();
        state_ = RUNNING;
    }
}

void ThreadBase::Run() {
    TRACE_FUNC();
    LOGI("enter thread: %u",std::this_thread::get_id());

    while (!stop_flag_) {
        Process();
        std::this_thread::sleep_for(std::chrono::nanoseconds(10));
        if (pause_flag_) {
            std::unique_lock <std::mutex> lck(mutex_);
            while (pause_flag_) {
                condition_.wait(lck);
            }
            lck.unlock();
        }
    }
    pause_flag_ = false;
    stop_flag_ = false;

    LOGI("exit thread: %d",std::this_thread::get_id());
}
