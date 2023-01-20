#include "person_impl.h"

PersonImpl::PersonImpl(const std::string& name, std::string& birthday,
                       std::string& addr)
    : name_(name), birth_date_(birthday), address_(addr) {}

std::string PersonImpl::Name() const { return name_; }
std::string PersonImpl::BirthDate() const { return birth_date_; }
std::string PersonImpl::Address() const { return address_; }