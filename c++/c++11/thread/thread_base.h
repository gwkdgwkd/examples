#ifndef THREAD_BASE_H
#define THREAD_BASE_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

// 可以暂停的线程
class ThreadBase {
 public:
  ThreadBase();
  virtual ~ThreadBase();
  enum State {
    STOPED,   // 停止状态，包括从未启动过和启动后被停止
    RUNNING,  // 运行状态
    PAUSED    // 暂停状态
  };

  State State() const;
  void Start();
  void Stop();
  void Pause();
  void Resume();

 protected:
  virtual void Process() = 0;

 private:
  void Run();

 private:
  std::thread *thread_;
  std::mutex mutex_;
  std::condition_variable condition_;
  std::atomic_bool pause_flag_;
  std::atomic_bool stop_flag_;
  enum State state_;
};

#endif  // THREAD_BASE_H
