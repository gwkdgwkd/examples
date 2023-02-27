#include <time.h>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

std::string printTime() {
  time_t curtime = time(NULL);
  tm *ptm = localtime(&curtime);
  char buf[64];
  sprintf(buf, "%d/%02d/%02d %02d:%02d:%02d", ptm->tm_year + 1900,
          ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min,
          ptm->tm_sec);

  std::string ret(buf);
  ret += " ";

  return ret;
}

std::mutex x;
std::condition_variable cv;
bool flag;

namespace n1 {
void f() {
  std::cout << printTime() << "bengin A" << std::endl;
  std::unique_lock<std::mutex> lck(x);
  while (!(flag == true)) {
    cv.wait(lck);
    std::cout << printTime() << "wait return" << std::endl;
  }
  std::cout << printTime() << "End A" << std::endl;
}

namespace test1 {
void func() {
  flag = false;
  std::thread t(f);
  t.join();

  // 2022/10/08 15:26:43 bengin A
  // ^C

  // 条件不为真，一直阻塞。
}
}  // namespace test1

namespace test2 {
void func() {
  flag = true;
  std::thread t(f);
  t.join();

  // 2022/10/08 15:32:03 bengin A
  // 2022/10/08 15:32:03 End A

  // 条件为真，不执行wait函数，所以不阻塞。
}
}  // namespace test2

namespace test3 {
void func() {
  flag = false;
  std::thread t(f);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  flag = true;
  t.join();

  // 2022/10/08 15:33:55 bengin A
  // ^C

  // 条件不为真，一直阻塞。
  // 修改flag为true，但不发送notify，仍然阻塞。
}
}  // namespace test3

namespace test4 {
void func() {
  flag = false;
  std::thread t(f);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  cv.notify_one();
  t.join();

  // 2022/10/08 15:37:08 bengin A
  // 2022/10/08 15:37:09 wait return
  // ^C

  // 条件不为真，阻塞。
  // 发送notify，wait返回，但是条件仍然不满足，继续执行wait，阻塞。
}
}  // namespace test4

namespace test5 {
void func() {
  flag = false;
  std::thread t(f);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  flag = true;
  cv.notify_one();
  t.join();

  // 2022/10/08 15:50:25 bengin A
  // 2022/10/08 15:50:26 wait return
  // 2022/10/08 15:50:26 End A

  // 条件不为真，阻塞。
  // 修改flag为true，发送notify，wait返回，条件满足，不wait，继续执行。
}
}  // namespace test5
}  // namespace n1

namespace n2 {
void f(std::function<bool()> pred) {
  std::cout << printTime() << "bengin A" << std::endl;
  std::unique_lock<std::mutex> lck(x);
  cv.wait(lck, pred);

  std::cout << printTime() << "End A" << std::endl;
}

namespace test1 {
void func() {
  flag = false;
  std::thread t(f, []() { return flag == true; });
  t.join();

  // 2022/10/08 15:45:45 bengin A
  // ^C

  // wait第二个参数返回值不为真，一直阻塞。
}
}  // namespace test1
namespace test2 {
void func() {
  flag = true;
  std::thread t(f, []() { return flag == true; });
  t.join();

  // 2022/10/08 15:46:33 bengin A
  // 2022/10/08 15:46:33 End A

  // wait第二个参数返回值为真，直接返回，不阻塞。
}
}  // namespace test2
namespace test3 {
void func() {
  flag = false;
  std::thread t(f, []() { return flag == true; });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  flag = true;
  t.join();

  // 2022/10/08 15:47:29 bengin A
  // ^C

  // wait第二个参数返回值不为真，一直阻塞。
  // 修改flag为true，但不发送notify，仍然阻塞。
}
}  // namespace test3

namespace test4 {
void func() {
  flag = false;
  std::thread t(f, []() { return flag == true; });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  cv.notify_one();
  t.join();

  // 2022/10/08 15:47:59 bengin A
  // ^C

  // wait第二个参数不为真，阻塞。
  // 发送notify，wait返回，但是第二个参数返回值仍然不为真，继续wait，阻塞。
}
}  // namespace test4

namespace test5 {
void func() {
  flag = false;
  std::thread t(f, []() { return flag == true; });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  flag = true;
  cv.notify_one();
  t.join();

  // 2022/10/08 15:52:28 bengin A
  // 2022/10/08 15:52:29 End A

  // wait第二个参数返回值不为真，阻塞。
  // flag改为true，发送notify，wait返回，第二个参数返回值为真，wait返回，不阻塞。
}
}  // namespace test5
}  // namespace n2

namespace n3 {
void f(std::function<bool()> pred) {
  std::cout << printTime() << "bengin A" << std::endl;
  std::unique_lock<std::mutex> lck(x);
  if (cv.wait_for(lck, std::chrono::seconds(5), pred)) {
    std::cout << printTime() << "finish waiting, wait_for return true"
              << std::endl;
  } else {
    std::cout << printTime() << "time out, wait_for return false" << std::endl;
  }
  std::cout << printTime() << "End A" << std::endl;
}
namespace test1 {
void func() {
  flag = true;
  std::thread t(f, []() { return flag == true; });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  t.join();

  // 2022/10/08 14:50:12 bengin A
  // 2022/10/08 14:50:12 finish waiting, wait_for return true
  // 2022/10/08 14:50:12 End A

  // wait_for第三个参数是真，不阻塞，wait_for返回true。
}
}  // namespace test1

namespace test2 {
void func() {
  flag = false;
  std::thread t(f, []() { return flag == true; });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  t.join();

  // 2022/10/08 14:51:22 bengin A
  // 2022/10/08 14:51:27 time out, wait_for return false
  // 2022/10/08 14:51:27 End A

  // wait_for第三个参数是假，阻塞，未修改flag。
  // 超时后wait_for返回值与第三个参数此时的返回值相同，返回false。
}
}  // namespace test2

namespace test3 {
void func() {
  flag = false;
  std::thread t(f, []() { return flag == true; });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  cv.notify_one();
  t.join();

  // 2022/10/08 15:03:17 bengin A
  // 2022/10/08 15:03:22 time out, wait_for return false
  // 2022/10/08 15:03:22 End A

  // wait_for第三个参数是假，阻塞，未修改flag，但发送notify。
  // 收到notify后，wait_for的第三个参数还是假，wait_for还会继续阻塞。
  // 超时后wait_for返回值与第三个参数返回值相同，返回false。
}
}  // namespace test3

namespace test4 {
void func() {
  flag = false;
  std::thread t(f, []() { return flag == true; });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  flag = true;
  t.join();

  // 2022/10/08 14:53:38 bengin A
  // 2022/10/08 14:53:43 finish waiting, wait_for return true
  // 2022/10/08 14:53:43 End A

  // wait_for第三个参数是假，阻塞，修改flag为true。
  // 超时后wait_for返回值与第三个参数此时的返回值相同，返回true。
}
}  // namespace test4

namespace test5 {
void func() {
  flag = false;
  std::thread t(f, []() { return flag == true; });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  flag = true;
  cv.notify_one();

  // 2022/10/08 15:08:06 bengin A
  // 2022/10/08 15:08:07 finish waiting, wait_for return true
  // 2022/10/08 15:08:07 End A

  // wait_for第三个参数是假，阻塞，修改flag为true，并发送notify。
  // 立即唤醒，第三个参数此时是真，wait_for返回true。
}
}  // namespace test5
}  // namespace n3

namespace n4 {
void f(std::function<bool()> pred) {
  std::cout << printTime() << "bengin A" << std::endl;
  std::unique_lock<std::mutex> lck(x);
  auto now = std::chrono::system_clock::now();
  if (cv.wait_until(lck, now + std::chrono::seconds(5), pred)) {
    std::cout << printTime() << "finish waiting, wait_until return true"
              << std::endl;
  } else {
    std::cout << printTime() << "time out, wait_until return false"
              << std::endl;
  }
  std::cout << printTime() << "End A" << std::endl;
}
namespace test1 {
void func() {
  flag = true;
  std::thread t(f, []() { return flag == true; });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  t.join();

  // 2022/10/08 16:07:17 bengin A
  // 2022/10/08 16:07:17 finish waiting, wait_until return true
  // 2022/10/08 16:07:17 End A

  // wait_until第三个参数是真，不阻塞，wait_until返回true。
}
}  // namespace test1

namespace test2 {
void func() {
  flag = false;
  std::thread t(f, []() { return flag == true; });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  t.join();

  // 2022/10/08 16:08:01 bengin A
  // 2022/10/08 16:08:06 time out, wait_until return false
  // 2022/10/08 16:08:06 End A

  // wait_until第三个参数是假，阻塞，未修改flag。
  // 超时后wait_until返回值与第三个参数此时的返回值相同，返回false。
}
}  // namespace test2

namespace test3 {
void func() {
  flag = false;
  std::thread t(f, []() { return flag == true; });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  cv.notify_one();
  t.join();

  // 2022/10/08 16:08:47 bengin A
  // 2022/10/08 16:08:52 time out, wait_until return false
  // 2022/10/08 16:08:52 End A

  // wait_until第三个参数是假，阻塞，未修改flag，但发送notify。
  // 收到notify后，wait_until的第三个参数还是假，wait_until还会继续阻塞。
  // 超时后wait_until返回值与第三个参数此时的返回值相同，返回false。
}
}  // namespace test3

namespace test4 {
void func() {
  flag = false;
  std::thread t(f, []() { return flag == true; });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  flag = true;
  t.join();

  // 2022/10/08 16:09:58 bengin A
  // 2022/10/08 16:10:03 finish waiting, wait_until return true
  // 2022/10/08 16:10:03 End A

  // wait_until第三个参数是假，阻塞，修改flag为true。
  // 超时后wait_until返回值与第三个参数此时的返回值相同，返回true。
}
}  // namespace test4
namespace test5 {
void func() {
  flag = false;
  std::thread t(f, []() { return flag == true; });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  flag = true;
  cv.notify_one();
  t.join();

  // 2022/10/08 16:10:36 bengin A
  // 2022/10/08 16:10:37 finish waiting, wait_until return true
  // 2022/10/08 16:10:37 End A

  // wait_until第三个参数是假，阻塞，修改flag为true，并发送notify。
  // 立即唤醒，第三个参数此时是真，wait_until返回true。
}
}  // namespace test5
}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 19]" << std::endl;
    return 0;
  }

  int type = atoi(argv[1]);
  switch (type) {
    case 0:
      n1::test1::func();
      break;
    case 1:
      n1::test2::func();
      break;
    case 2:
      n1::test3::func();
      break;
    case 3:
      n1::test4::func();
      break;
    case 4:
      n1::test5::func();
      break;
    case 5:
      n2::test1::func();
      break;
    case 6:
      n2::test2::func();
      break;
    case 7:
      n2::test3::func();
      break;
    case 8:
      n2::test4::func();
      break;
    case 9:
      n2::test5::func();
      break;
    case 10:
      n3::test1::func();
      break;
    case 11:
      n3::test2::func();
      break;
    case 12:
      n3::test3::func();
      break;
    case 13:
      n3::test4::func();
      break;
    case 14:
      n3::test5::func();
      break;
    case 15:
      n4::test1::func();
      break;
    case 16:
      n4::test2::func();
      break;
    case 17:
      n4::test3::func();
      break;
    case 18:
      n4::test4::func();
      break;
    case 19:
      n4::test5::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}
