#include "person.h"

Person::Person(const std::string& name, std::string& birthday,
               std::string& addr)
    : name_(name), birth_date_(birthday), address_(addr) {}

std::string Person::Name() const { return name_; }
std::string Person::BirthDate() const { return birth_date_; }
std::string Person::Address() const { return address_; }