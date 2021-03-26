#include <time.h>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

using namespace std;

string printTime() {
  time_t curtime = time(NULL);
  tm *ptm = localtime(&curtime);
  char buf[64];
  sprintf(buf, "%d/%02d/%02d %02d:%02d:%02d", ptm->tm_year + 1900,
          ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min,
          ptm->tm_sec);

  string ret(buf);
  ret += " ";

  return ret;
}

mutex x;
condition_variable c;
bool flag = false;

void funcA() {
  cout << printTime() << "bengin A" << endl;
  std::unique_lock<std::mutex> lck(x);
  if (c.wait_for(lck, chrono::seconds(5), [] { return flag == true; })) {
    cout << printTime() << "finish waiting" << endl;
  } else {
    cout << printTime() << "time out" << endl;
  }
  cout << printTime() << "End A" << endl;
}
void funcB() {
  //   cout << printTime() << "bengin B" << endl;
  cout << printTime() << "bengin B, set flag to true" << endl;
  // 下面两句的顺序重要么？
  flag = true;
  c.notify_one();
  cout << printTime() << "End B, send notify" << endl;
}

int main() {
  thread x(funcA);
  this_thread::sleep_for(chrono::seconds(1));
  thread y(funcB);
  x.join();
  y.join();

  return 0;
}

// bool flag = false;
// if (c.wait_for(lck, chrono::seconds(5), [] { return flag == false; })) {
// wait_for第三个参数是真，不阻塞，wait_for返回true
// 2021/03/26 15:52:16 bengin A
// 2021/03/26 15:52:16 finish waiting
// 2021/03/26 15:52:16 End A
// real    0m0.003s
// user    0m0.003s
// sys     0m0.000s

// bool flag = false;
// if (c.wait_for(lck, chrono::seconds(5), [] { return flag == true; })) {
// wait_for第三个参数是假，阻塞，超时后第三个参数还是假，wait_for返回false
// 2021/03/26 15:54:39 bengin A
// 2021/03/26 15:54:44 time out
// 2021/03/26 15:54:44 End A
// real    0m5.003s
// user    0m0.000s
// sys     0m0.002s

// bool flag = false;
// if (c.wait_for(lck, chrono::seconds(5), [] { return flag == true; })) {
// wait_for第三个参数是假，阻塞，发送notify，不修改flag，超时后第三个参数假，wait_for返回false
// 2021/03/26 16:14:40 bengin A
// 2021/03/26 16:14:41 bengin B
// 2021/03/26 16:14:41 End B, send notify
// 2021/03/26 16:14:45 time out
// 2021/03/26 16:14:45 End A
// real    0m5.003s
// user    0m0.003s
// sys     0m0.001s

// bool flag = false;
// if (c.wait_for(lck, chrono::seconds(5), [] { return flag == true; })) {
// wait_for第三个参数是假，阻塞，其他线程修改flag为true，超时后第三个参数是真，wait_for返回true
// 2021/03/26 15:59:40 bengin A
// 2021/03/26 15:59:41 bengin B, set flag to true
// 2021/03/26 15:59:41 End B
// 2021/03/26 15:59:45 finish waiting
// 2021/03/26 15:59:45 End A
// real    0m5.003s
// user    0m0.000s
// sys     0m0.003s

// bool flag = false;
// if (c.wait_for(lck, chrono::seconds(5), [] { return flag == true; })) {
// wait_for第三个参数是假，阻塞，其他线程修改flag为true，并发送notify，立即唤醒，第三个参数是真，wait_for返回true
// 2021/03/26 16:01:53 bengin A
// 2021/03/26 16:01:54 bengin B, set flag to true
// 2021/03/26 16:01:54 End B, send notify
// 2021/03/26 16:01:54 finish waiting
// 2021/03/26 16:01:54 End A
// real    0m1.003s
// user    0m0.003s
// sys     0m0.000s