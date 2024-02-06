#include "real_person.h"

RealPerson::RealPerson(const std::string& name, std::string& birthday,
                       std::string& addr)
    : name_(name), birth_date_(birthday), address_(addr) {}

std::string RealPerson::Name() const { return name_; }
std::string RealPerson::BirthDate() const { return birth_date_; }
std::string RealPerson::Address() const { return address_; }