#include "person.h"
#include "real_person.h"

Person::~Person() {}

std::shared_ptr<Person> Person::create(const std::string& name,
                                       std::string& birthday,
                                       std::string& addr) {
  return std::make_shared<RealPerson>(name, birthday, addr);
}