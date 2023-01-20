#include <string>
#include <memory>

class PersonImpl;
class Person {
 public:
  Person(const std::string& name, std::string& birthday, std::string& addr);
  std::string Name() const;
  std::string BirthDate() const;
  std::string Address() const;

 private:
  std::shared_ptr<PersonImpl> person_impl_;
};