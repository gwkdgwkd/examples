#include "person.h"
#include "person_impl.h"

Person::Person(const std::string& name, std::string& birthday,
               std::string& addr) {
  person_impl_ = std::make_shared<PersonImpl>(name, birthday, addr);
}

std::string Person::Name() const { return person_impl_->Name(); }
std::string Person::BirthDate() const { return person_impl_->BirthDate(); }
std::string Person::Address() const { return person_impl_->Address(); }