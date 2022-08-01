#include "thread_base.h"

#include <iostream>

ThreadBase::ThreadBase()
    : thread_(nullptr), pause_flag_(false), stop_flag_(false), state_(STOPED) {}

ThreadBase::~ThreadBase() { Stop(); }

enum ThreadBase::State ThreadBase::State() const { return state_; }

void ThreadBase::Start() {
  if (thread_ == nullptr) {
    thread_ = new std::thread(&ThreadBase::Run, this);
    pause_flag_ = false;
    stop_flag_ = false;
    state_ = RUNNING;
  }
}

void ThreadBase::Stop() {
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
  if (thread_ != nullptr) {
    pause_flag_ = true;
    state_ = PAUSED;
  }
}

void ThreadBase::Resume() {
  if (thread_ != nullptr) {
    pause_flag_ = false;
    condition_.notify_all();
    state_ = RUNNING;
  }
}

void ThreadBase::Run() {
  while (!stop_flag_) {
    if (pause_flag_) {
      std::unique_lock<std::mutex> lck(mutex_);
      while (pause_flag_) {
        condition_.wait(lck);
      }
      lck.unlock();  // 不用这句也可以吧
    }
    Process();
    std::this_thread::sleep_for(std::chrono::nanoseconds(10));
  }
  pause_flag_ = false;
  stop_flag_ = false;
}
