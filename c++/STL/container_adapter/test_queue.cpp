#include <algorithm>  // For min_element & max_element
#include <iomanip>    // For stream manipulators
#include <iostream>   // For standard streams
#include <numeric>    // For accumulate()
#include <queue>
#include <random>  // For random number generation
#include <string>  // For string class
#include <vector>  // For vector container

class Customer {
 protected:
  size_t service_t{};  // 顾客结账需要的时间
 public:
  explicit Customer(size_t st = 10) : service_t{st} {}
  // 模拟随着时间的变化，顾客结账所需时间也会减短
  Customer& time_decrement() {
    if (service_t > 0) --service_t;
    return *this;
  }
  bool done() const { return service_t == 0; }
};

class Checkout {
 private:
  std::queue<Customer> customers;  // 该队列等到结账的顾客数量
 public:
  void add(const Customer& customer) { customers.push(customer); }
  size_t qlength() const { return customers.size(); }

  void time_increment() {
    if (!customers.empty()) {
      // 有顾客正在等待结账，如果顾客结账了，就出队列
      if (customers.front().time_decrement().done()) customers.pop();
    }
  }
  bool operator<(const Checkout& other) const {
    return qlength() < other.qlength();
  }
  bool operator>(const Checkout& other) const {
    return qlength() > other.qlength();
  }
};

using std::string;
using distribution = std::uniform_int_distribution<>;
// 以横向柱形图的方式输出每个服务时间出现的次数
void histogram(const std::vector<int>& v, int min) {
  string bar(60, '*');
  for (size_t i{}; i < v.size(); ++i) {
    std::cout << std::setw(3) << i + min << " "  // 结账等待时间为 index + min
              << std::setw(4) << v[i] << " "  // 输出出现的次数
              << bar.substr(0, v[i])
              << (v[i] > static_cast<int>(bar.size()) ? "..." : "")
              << std::endl;
  }
}
int main() {
  std::random_device random_n;
  // 设置最大和最小的结账时间，以分钟为单位
  int service_t_min{2}, service_t_max{15};
  distribution service_t_d{service_t_min, service_t_max};
  // 设置在超市开业时顾客的人数
  int min_customers{15}, max_customers{20};
  distribution n_1st_customers_d{min_customers, max_customers};
  // 设置顾客到达的最大和最小的时间间隔
  int min_arr_interval{1}, max_arr_interval{5};
  distribution arrival_interval_d{min_arr_interval, max_arr_interval};
  size_t n_checkouts{};
  std::cout << "输入超市中结账柜台的数量：";
  std::cin >> n_checkouts;
  if (!n_checkouts) {
    std::cout << "结账柜台的数量必须大于 0，这里将默认设置为 1" << std::endl;
    n_checkouts = 1;
  }
  std::vector<Checkout> checkouts{n_checkouts};
  std::vector<int> service_times(service_t_max - service_t_min + 1);
  // 等待超市营业的顾客人数
  int count{n_1st_customers_d(random_n)};
  std::cout << "等待超市营业的顾客人数：" << count << std::endl;
  int added{};
  int service_t{};
  while (added++ < count) {
    service_t = service_t_d(random_n);
    std::min_element(std::begin(checkouts), std::end(checkouts))
        ->add(Customer(service_t));
    ++service_times[service_t - service_t_min];
  }
  size_t time{};
  const size_t total_time{600};  // 设置超市持续营业的时间
  size_t longest_q{};            // 等待结账最长队列的长度
  // 新顾客到达的时间
  int new_cust_interval{arrival_interval_d(random_n)};
  // 模拟超市运转的过程
  while (time < total_time) {
    ++time;  // 时间增长
    // 新顾客到达
    if (--new_cust_interval == 0) {
      service_t = service_t_d(random_n);  // 设置顾客结账所需要的时间
      std::min_element(std::begin(checkouts), std::end(checkouts))
          ->add(Customer(service_t));
      ++service_times[service_t - service_t_min];  // 记录结账需要等待的时间
      // 记录最长队列的长度
      for (auto& checkout : checkouts)
        longest_q = std::max(longest_q, checkout.qlength());
      new_cust_interval = arrival_interval_d(random_n);
    }
    // 更新每个队列中第一个顾客的结账时间
    for (auto& checkout : checkouts) checkout.time_increment();
  }
  std::cout << "最大的队列长度为：" << longest_q << std::endl;
  std::cout << "\n各个结账时间出现的次数：:\n";
  histogram(service_times, service_t_min);
  std::cout << "\n总的顾客数：" << std::accumulate(std::begin(service_times),
                                                   std::end(service_times), 0)
            << std::endl;
  return 0;
}