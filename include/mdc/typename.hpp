#ifndef TYPENAME_HPP
#define TYPENAME_HPP
#include <string>
#if defined (__GNUG__) || defined (__clang__)
#include <cxxabi.h>
#include <cstdlib>
namespace mdc {
template <class T> std::string type_name() {
  std::string name = typeid (T).name();
  int stat;
  char* buff = abi::__cxa_demangle(name.c_str(), NULL, NULL, &stat);
  if (!stat) {
    name = buff;
    std::free(buff);
  }
  return name;
}
}
#else
#include <cstdlib>
template <class T> std::string type_name() {return typeid (T).name();}
#endif
#endif // TYPENAME_HPP
