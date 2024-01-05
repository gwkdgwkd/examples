#include <cxxabi.h>

#include <string>

inline const std::string GetClearName(const char* name) {
  int status = -1;
  char* clearName = abi::__cxa_demangle(name, NULL, NULL, &status);
  const char* const demangledName = (status == 0) ? clearName : name;
  std::string ret_val(demangledName);
  free(clearName);
  return ret_val;
}

template <typename T>
void printType(T&& val) {
  std::cout << typeid(T).name() << " : " << GetClearName(typeid(T).name())
            << std::endl;
}