#include <string>

#include "person.h"

class RealPerson : public Person {
 public:
  RealPerson(const std::string& name, std::string& birthday, std::string& addr);
  
  std::string Name() const override;
  std::string BirthDate() const override;
  std::string Address() const override;

 private:
  std::string name_;
  std::string birth_date_;
  std::string address_;

  // int add;
};