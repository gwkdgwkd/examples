#ifndef PERSON_H
#define PERSON_H

#include <memory>
#include <string>

class Person {
 public:
  virtual ~Person();
  virtual std::string Name() const = 0;
  virtual std::string BirthDate() const = 0;
  virtual std::string Address() const = 0;

  static std::shared_ptr<Person> create(const std::string& name,
                                        std::string& birthday,
                                        std::string& addr);
};

#endif