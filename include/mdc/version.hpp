#ifndef VERSION_HPP
#define VERSION_HPP
#include <iostream>
#include <sstream>
#include <string>
namespace mdc {
  struct version {
    std::size_t major;
    std::size_t minor;
    std::size_t patch;
    operator std::string() const {return std::to_string(major) + '.' + std::to_string(minor) + '.' + std::to_string(patch);}
    std::string to_string() const {return std::to_string(major) + '.' + std::to_string(minor) + '.' + std::to_string(patch);}
  };
  inline constexpr bool operator<(const version& lhs, const version& rhs) {return lhs.major < rhs.major || lhs.minor < rhs.minor || lhs.patch < rhs.patch;}
  inline constexpr bool operator>(const version& lhs, const version& rhs) {return lhs.major > rhs.major || lhs.minor > rhs.minor || lhs.patch > rhs.patch;}
  inline constexpr bool operator<=(const version& lhs, const version& rhs) {return !(lhs.major > rhs.major || lhs.minor > rhs.minor || lhs.patch > rhs.patch);}
  inline constexpr bool operator>=(const version& lhs, const version& rhs) {return !(lhs.major < rhs.major || lhs.minor < rhs.minor || lhs.patch < rhs.patch);}
  inline constexpr bool operator==(const version& lhs, const version& rhs) {return lhs.major == rhs.major && lhs.minor == rhs.minor && lhs.patch == rhs.patch;}
  inline constexpr bool operator!=(const version& lhs, const version& rhs) {return lhs.major != rhs.major || lhs.minor != rhs.minor || lhs.patch != rhs.patch;}
  inline std::istream& operator>>(std::istream& is, version& v) {
    is >> v.major;
    char c;
    if (!is.get(c) || c != '.') return is;
    is >> v.minor;
    if (!is.get(c) || c != '.') return is;
    is >> v.patch;
    return is;
  }
  inline std::ostream& operator<<(std::ostream& os, const version& v) {
    os << v.major << '.' << v.minor << '.' << v.patch;
    return os;
  }
  template <std::size_t size> struct r_version {
    std::size_t value;
    r_version<size - 1> next;
    operator std::string() const {return std::to_string(value) + '.' + next.to_string();}
    std::string to_string() const {return std::to_string(value) + '.' + next.to_string();}
  };
  template <> struct r_version<1> {
    std::size_t value;
    operator std::string() const {return std::to_string(value);}
    std::string to_string() const {return std::to_string(value);}
  };
  template <std::size_t l> constexpr inline bool operator<(const r_version<l>& lhs, const r_version<l>& rhs) {return lhs.value < rhs.value || lhs.next < rhs.next;}
  template <> constexpr inline bool operator<(const r_version<1>& lhs, const r_version<1>& rhs) {return lhs.value < rhs.value;}
  template <std::size_t l> constexpr inline bool operator>(const r_version<l>& lhs, const r_version<l>& rhs) {return lhs.value > rhs.value || lhs.next > rhs.next;}
  template <> constexpr inline bool operator>(const r_version<1>& lhs, const r_version<1>& rhs) {return lhs.value > rhs.value;}
  template <std::size_t l> constexpr bool operator<=(const r_version<l>& lhs, const r_version<l>& rhs) {return !(lhs > rhs);}
  template <std::size_t l> constexpr bool operator>=(const r_version<l>& lhs, const r_version<l>& rhs) {return !(lhs < rhs);}
  template <std::size_t l> constexpr inline bool operator==(const r_version<l>& lhs, const r_version<l>& rhs) {return lhs.value == rhs.value && lhs.next == rhs.next;}
  template <> constexpr inline bool operator==(const r_version<1>& lhs, const r_version<1>& rhs) {return lhs.value == rhs.value;}
  template <std::size_t l> constexpr inline bool operator!=(const r_version<l>& lhs, const r_version<l>& rhs) {return lhs.value != rhs.value || lhs.next != rhs.next;}
  template <> constexpr inline bool operator!=(const r_version<1>& lhs, const r_version<1>& rhs) {return lhs.value != rhs.value;}
  template <std::size_t l> inline std::istream& operator>>(std::istream& is, r_version<l>& v) {
    char c;
    is >> v.value;
    if (!is.get(c) || c != '.') return is;
    return (is >> v.value);
  }
  template <> inline std::istream& operator>>(std::istream& is, r_version<1>& v) {
    is >> v.value;
    return is;
  }
  template <std::size_t l>  inline std::ostream& operator<<(std::ostream& os, const r_version<l>& v) {
    os << v.value << '.' << v.next;
    return os;
  }
  template <> inline std::ostream& operator<<(std::ostream& os, const r_version<1>& v) {
    os << v.value;
    return os;
  }
  namespace {
    constexpr bool is_digit(char c) {return c <= '9' && c >= '0';}
    constexpr unsigned long stoi_impl(const char* str, unsigned long value, std::size_t& len) noexcept {return len ? is_digit(*str) ? stoi_impl(str + 1, (*str - '0') + value * 10, --len) : value : value;}
    constexpr unsigned long stoi(const char* str, std::size_t& len) noexcept {return stoi_impl(str, 0, len);}
//    constexpr std::pair<unsigned long, unsigned long> stoi_impl(const char* str, unsigned long value, unsigned long idx) noexcept {return *str ? is_digit(*str) ? stoi_impl(str + 1, (*str - '0') + value * 10, idx + 1) : std::pair{value, idx} : std::pair{value, idx};}
//    constexpr std::pair<unsigned long, unsigned long> stoi(const char* str) noexcept {return stoi_impl(str, 0, 0);}
  }
  inline namespace literals {
    inline namespace version_literals {
      constexpr version operator""_v(const char* in, std::size_t sz) {
        std::size_t p = sz;
        version v{stoi(in, p), 0, 0};
        --p;
        v.minor = stoi(in + sz - p, p);
        --p;
        v.patch = stoi(in + sz - p, p);
        if (sz) return {};
        return v;
      }
    }
  }
}
#endif // VERSION_HPP
