#ifndef CONSTEXPR_HPP
#define CONSTEXPR_HPP
namespace mdc {
  template <class T, class U> constexpr T inline cpow(const T& base, const U& exponent) {return exponent ? base * cpow(base, exponent - 1) : 1;}
}
#endif // CONSTEXPR_HPP
