#ifndef INTEGER_HPP
#define INTEGER_HPP
#include <utility>
namespace mdc {
  #ifdef __clang__
  #if __clang_major__ < 14
  #define _BitInt(X) _ExtInt(X)
  #endif
  template <long sz, bool is_unsigned = false> struct integer_selector {using type = _BitInt(sz);};
  template <long sz> struct integer_selector<sz, true> {using type = unsigned _BitInt(sz);};
  #else
  template <long sz, bool is_unsigned = false> struct integer_selector;
  #define INT_HELPER(SIZE) template <> struct integer_selector<SIZE, false> {using type = int ## SIZE ## _t;};
  #define UINT_HELPER(SIZE) template <> struct integer_selector<SIZE, true> {using type = uint ## SIZE ## _t;};
  INT_HELPER(8)
  INT_HELPER(16)
  INT_HELPER(32)
  INT_HELPER(64)
  UINT_HELPER(8)
  UINT_HELPER(16)
  UINT_HELPER(32)
  UINT_HELPER(64)
  #undef INT_HELPER
  #undef UINT_HELPER
  #endif
  template <long sz, bool is_unsigned = false> using integer = typename integer_selector<sz, is_unsigned>::type;
  template <long sz> using uinteger = integer<sz, true>;
}
#endif
