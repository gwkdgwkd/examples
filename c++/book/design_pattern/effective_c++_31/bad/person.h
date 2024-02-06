#include <string>

class Person {
 public:
  Person(const std::string& name, std::string& birthday, std::string& addr);
  std::string Name() const;
  std::string BirthDate() const;
  std::string Address() const;

 private:
  std::string name_;
  std::string birth_date_;
  std::string address_;

  // int add;
};