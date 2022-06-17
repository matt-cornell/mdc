#ifndef MDC_HOME_HPP
#define MDC_HOME_HPP
#include <cstdlib>
#include <string>
#if defined(_WIN32) || defined(_WIN64)
namespace mdc {
  inline std::string home_dir() {
    if (auto ptr = std::getenv("USERPROFILE")) return ptr;
    if (auto hd = std::getenv("HOMEDRIVE")) {
      if (auto hp = std::getenv("HOMEPATH")) return std::string(hd) + hp;
      else return "";
    }
    else return "";
  }
}
#elif defined(__unix__) || defined(__linux__) || defined(__APPLE__)
#define MDC_HAS_HOME
#include <unistd.h>
#include <pwd.h>
namespace mdc {
  inline std::string home_dir() {
    if (auto ptr = std::getenv("HOME")) return ptr;
    return getpwuid(getuid())->pw_dir;
  }
}
#endif
#endif