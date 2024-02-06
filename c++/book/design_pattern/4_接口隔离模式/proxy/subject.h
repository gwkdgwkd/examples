#ifndef SUBJECT_H
#define SUBJECT_H

class Subject {
 public:
  virtual ~Subject() = default;
  virtual void Request() = 0;

 protected:
  Subject() = default;
};
#endif  // SUBJECT_H