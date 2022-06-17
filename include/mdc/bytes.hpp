#ifndef BYTES_HPP
#define BYTES_HPP
#include <array>
#include <cmath>
#include <iostream>
#include <iomanip>
namespace mdc {
  struct bytes {
    unsigned long count;
    bytes() = default;
    constexpr inline bytes(const unsigned long count) : count(count) {}
    constexpr inline operator unsigned long&() {return count;}
    constexpr inline operator const unsigned long&() const {return count;}
  };
  const std::array<const char*, 7> sizes = {"B", "kB", "MB", "GB", "TB", "EB", "ZB"};
  std::ostream& operator<<(std::ostream& os, const bytes& size) {
    auto sz = std::log10(size.count) / 3;
    os << std::setprecision(1) << (double)size.count / std::pow(10, sz > sizes.size() ? sizes.size() : sz) << sizes[sz > sizes.size() ? sizes.size() : sz];
    return os;
  }
  inline namespace literals {
    inline namespace byte_literals {
      constexpr bytes operator""_b(unsigned long long count) {return count;}
      constexpr bytes operator""_kb(unsigned long long count) {return count * 1'000;}
      constexpr bytes operator""_mb(unsigned long long count) {return count * 1'000'000;}
      constexpr bytes operator""_gb(unsigned long long count) {return count * 1'000'000'000;}
      constexpr bytes operator""_tb(unsigned long long count) {return count * 1'000'000'000'000;}
    }
  }
}
#endif // BYTES_HPP
