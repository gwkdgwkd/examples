#include <time.h>
#include <condition_variable>
#include <functional>
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
condition_variable cv;
bool flag;

void func1() {
  cout << printTime() << "bengin A" << endl;
  std::unique_lock<std::mutex> lck(x);
  while (!(flag == true)) {
    cv.wait(lck);
    cout << printTime() << "wait return" << endl;
  }
  cout << printTime() << "End A" << endl;
}

void func2(function<bool()> pred) {
  cout << printTime() << "bengin A" << endl;
  std::unique_lock<std::mutex> lck(x);
  cv.wait(lck, pred);

  cout << printTime() << "End A" << endl;
}

void func3(function<bool()> pred) {
  cout << printTime() << "bengin A" << endl;
  std::unique_lock<std::mutex> lck(x);
  if (cv.wait_for(lck, chrono::seconds(5), pred)) {
    cout << printTime() << "finish waiting, wait_for return true" << endl;
  } else {
    cout << printTime() << "time out, wait_for return false" << endl;
  }
  cout << printTime() << "End A" << endl;
}

void func4(function<bool()> pred) {
  cout << printTime() << "bengin A" << endl;
  std::unique_lock<std::mutex> lck(x);
  auto now = std::chrono::system_clock::now();
  if (cv.wait_until(lck, now + chrono::seconds(5), pred)) {
    cout << printTime() << "finish waiting, wait_until return true" << endl;
  } else {
    cout << printTime() << "time out, wait_until return false" << endl;
  }
  cout << printTime() << "End A" << endl;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
    return 0;
  }

  int type = argv[1][0] - '0';
  if (argv[1][1]) {
    type = (argv[1][0] - '0') * 10 + argv[1][1] - '0';
  } else {
    type = argv[1][0] - '0';
  }
  switch (type) {
    case 0: {
      flag = false;
      thread t(func1);
      t.join();
      break;

      // 2022/10/08 15:26:43 bengin A
      // ^C

      // 条件不为真，一直阻塞。
    }
    case 1: {
      flag = true;
      thread t(func1);
      t.join();
      break;

      // 2022/10/08 15:32:03 bengin A
      // 2022/10/08 15:32:03 End A

      // 条件为真，不执行wait函数，所以不阻塞。
    }
    case 2: {
      flag = false;
      thread t(func1);
      this_thread::sleep_for(chrono::seconds(1));
      flag = true;
      t.join();
      break;

      // 2022/10/08 15:33:55 bengin A
      // ^C

      // 条件不为真，一直阻塞。
      // 修改flag为true，但不发送notify，仍然阻塞。
    }
    case 3: {
      flag = false;
      thread t(func1);
      this_thread::sleep_for(chrono::seconds(1));
      cv.notify_one();
      t.join();
      break;

      // 2022/10/08 15:37:08 bengin A
      // 2022/10/08 15:37:09 wait return
      // ^C

      // 条件不为真，阻塞。
      // 发送notify，wait返回，
      // 但是条件仍然不满足，继续执行wait，阻塞。
    }
    case 4: {
      flag = false;
      thread t(func1);
      this_thread::sleep_for(chrono::seconds(1));
      flag = true;
      cv.notify_one();
      t.join();
      break;

      // 2022/10/08 15:50:25 bengin A
      // 2022/10/08 15:50:26 wait return
      // 2022/10/08 15:50:26 End A

      // 条件不为真，阻塞。
      // 修改flag为true，发送notify，wait返回，
      // 条件满足，不执行wait，继续执行后面的操作。
    }
    case 5: {
      flag = false;
      thread t(func2, []() { return flag == true; });
      t.join();
      break;

      // 2022/10/08 15:45:45 bengin A
      // ^C

      // wait第二个参数返回值不为真，一直阻塞。
    }
    case 6: {
      flag = true;
      thread t(func2, []() { return flag == true; });
      t.join();
      break;

      // 2022/10/08 15:46:33 bengin A
      // 2022/10/08 15:46:33 End A

      // wait第二个参数返回值为真，直接返回，不阻塞。
    }
    case 7: {
      flag = false;
      thread t(func2, []() { return flag == true; });
      this_thread::sleep_for(chrono::seconds(1));
      flag = true;
      t.join();
      break;

      // 2022/10/08 15:47:29 bengin A
      // ^C

      // wait第二个参数返回值不为真，一直阻塞。
      // 修改flag为true，但不发送notify，仍然阻塞。
    }
    case 8: {
      flag = false;
      thread t(func2, []() { return flag == true; });
      this_thread::sleep_for(chrono::seconds(1));
      cv.notify_one();
      t.join();
      break;

      // 2022/10/08 15:47:59 bengin A
      // ^C

      // wait第二个参数不为真，阻塞。
      // 发送notify，wait返回，
      // 但是第二个参数返回值仍然不为真，继续执行wait，阻塞。
    }
    case 9: {
      flag = false;
      thread t(func2, []() { return flag == true; });
      this_thread::sleep_for(chrono::seconds(1));
      flag = true;
      cv.notify_one();
      t.join();
      break;

      // 2022/10/08 15:52:28 bengin A
      // 2022/10/08 15:52:29 End A

      // wait第二个参数返回值不为真，阻塞。
      // 修改flag为true，发送notify，wait返回，
      // 第二个参数返回值为真，wait返回，不阻塞。
    }
    case 10: {
      flag = true;
      thread t(func3, []() { return flag == true; });
      this_thread::sleep_for(chrono::seconds(1));
      t.join();
      break;

      // 2022/10/08 14:50:12 bengin A
      // 2022/10/08 14:50:12 finish waiting, wait_for return true
      // 2022/10/08 14:50:12 End A

      // wait_for第三个参数是真，不阻塞，wait_for返回true。
    }
    case 11: {
      flag = false;
      thread t(func3, []() { return flag == true; });
      this_thread::sleep_for(chrono::seconds(1));
      t.join();
      break;

      // 2022/10/08 14:51:22 bengin A
      // 2022/10/08 14:51:27 time out, wait_for return false
      // 2022/10/08 14:51:27 End A

      // wait_for第三个参数是假，阻塞，未修改flag。
      // 超时后wait_for返回值与第三个参数此时的返回值（false）相同，
      // wait_for返回false。
    }
    case 12: {
      flag = false;
      thread t(func3, []() { return flag == true; });
      this_thread::sleep_for(chrono::seconds(1));
      cv.notify_one();
      t.join();
      break;

      // 2022/10/08 15:03:17 bengin A
      // 2022/10/08 15:03:22 time out, wait_for return false
      // 2022/10/08 15:03:22 End A

      // wait_for第三个参数是假，阻塞，未修改flag，但发送notify。
      // 收到notify后，wait_for的第三个参数还是假，wait_for还会继续阻塞。
      // 超时后wait_for返回值与第三个参数此时的返回值（false）相同，
      // wait_for返回false。
    }
    case 13: {
      flag = false;
      thread t(func3, []() { return flag == true; });
      this_thread::sleep_for(chrono::seconds(1));
      flag = true;
      t.join();
      break;

      // 2022/10/08 14:53:38 bengin A
      // 2022/10/08 14:53:43 finish waiting, wait_for return true
      // 2022/10/08 14:53:43 End A

      // wait_for第三个参数是假，阻塞，修改flag为true。
      // 超时后wait_for返回值与第三个参数此时的返回值（true）相同，
      // wait_for返回true。
    }
    case 14: {
      flag = false;
      thread t(func3, []() { return flag == true; });
      this_thread::sleep_for(chrono::seconds(1));
      flag = true;
      cv.notify_one();
      t.join();
      break;

      // 2022/10/08 15:08:06 bengin A
      // 2022/10/08 15:08:07 finish waiting, wait_for return true
      // 2022/10/08 15:08:07 End A

      // wait_for第三个参数是假，阻塞，修改flag为true，并发送notify。
      // 立即唤醒，第三个参数此时是真，wait_for返回true。
    }
    case 15: {
      flag = true;
      thread t(func4, []() { return flag == true; });
      this_thread::sleep_for(chrono::seconds(1));
      t.join();
      break;

      // 2022/10/08 16:07:17 bengin A
      // 2022/10/08 16:07:17 finish waiting, wait_until return true
      // 2022/10/08 16:07:17 End A

      // wait_until第三个参数是真，不阻塞，wait_until返回true。
    }
    case 16: {
      flag = false;
      thread t(func4, []() { return flag == true; });
      this_thread::sleep_for(chrono::seconds(1));
      t.join();
      break;

      // 2022/10/08 16:08:01 bengin A
      // 2022/10/08 16:08:06 time out, wait_until return false
      // 2022/10/08 16:08:06 End A

      // wait_until第三个参数是假，阻塞，未修改flag。
      // 超时后wait_until返回值与第三个参数此时的返回值（false）相同，
      // wait_until返回false。
    }
    case 17: {
      flag = false;
      thread t(func4, []() { return flag == true; });
      this_thread::sleep_for(chrono::seconds(1));
      cv.notify_one();
      t.join();
      break;

      // 2022/10/08 16:08:47 bengin A
      // 2022/10/08 16:08:52 time out, wait_until return false
      // 2022/10/08 16:08:52 End A

      // wait_until第三个参数是假，阻塞，未修改flag，但发送notify。
      // 收到notify后，wait_until的第三个参数还是假，wait_until还会继续阻塞。
      // 超时后wait_until返回值与第三个参数此时的返回值（false）相同，
      // wait_until返回false。
    }
    case 18: {
      flag = false;
      thread t(func4, []() { return flag == true; });
      this_thread::sleep_for(chrono::seconds(1));
      flag = true;
      t.join();
      break;

      // 2022/10/08 16:09:58 bengin A
      // 2022/10/08 16:10:03 finish waiting, wait_until return true
      // 2022/10/08 16:10:03 End A

      // wait_until第三个参数是假，阻塞，修改flag为true。
      // 超时后wait_until返回值与第三个参数此时的返回值（true）相同，
      // wait_until返回true。
    }
    case 19: {
      flag = false;
      thread t(func4, []() { return flag == true; });
      this_thread::sleep_for(chrono::seconds(1));
      flag = true;
      cv.notify_one();
      t.join();
      break;

      // 2022/10/08 16:10:36 bengin A
      // 2022/10/08 16:10:37 finish waiting, wait_until return true
      // 2022/10/08 16:10:37 End A

      // wait_until第三个参数是假，阻塞，修改flag为true，并发送notify。
      // 立即唤醒，第三个参数此时是真，wait_until返回true。
    }
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}
