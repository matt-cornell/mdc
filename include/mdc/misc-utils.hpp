#ifndef MISC_UTILS_HPP
#define MISC_UTILS_HPP
#include <cmath>
#include <iostream>
#include <type_traits>
namespace mdc {
  namespace detail {
    template <class L, class R> char mod(const L&, const R&) = delete;
    template <class L, class R> struct has_mod_op {
      static char8_t test(...);
      static char16_t test(decltype(std::declval<L>() % std::declval<R>()));
      enum {value = sizeof(decltype(test(std::declval<L>() % std::declval<R>())))};
    };
    template <class L, class R> struct has_mod_fn {
      static char8_t test(...);
      static char16_t test(decltype(mod(std::declval<L>(), std::declval<R>())));
      enum {value = sizeof(decltype(test(mod(std::declval<L>(), std::declval<R>()))))};
    };
    template <class L, class R> struct has_fmod_fn {
      static char8_t test(...);
      static char16_t test(decltype(fmod(std::declval<L>(), std::declval<R>())));
      enum {value = sizeof(decltype(test(fmod(std::declval<L>(), std::declval<R>()))))};
    };
  }
  template <class L, class R> requires (detail::has_mod_op<L, R>::value || detail::has_mod_fn<L, R>::value || detail::has_fmod_fn<L, R>::value) decltype(auto) modulus(L&& lhs, R&& rhs) {
    if constexpr (detail::has_mod_op<L, R>::value) return std::forward<L>(lhs) % std::forward<R>(rhs);
    else if constexpr (detail::has_mod_fn<L, R>::value) return mod(std::forward<L>(lhs), std::forward<R>(rhs));
    else return fmod(std::forward<L>(lhs), std::forward<R>(rhs));
  }
  template <class R, class... As> R donothing(As&&...) {return *(std::remove_reference_t<R>*)nullptr;}
  struct print_fn {
    std::ostream* os;
    std::ostream& (*term)(std::ostream&) = donothing<std::ostream&, std::ostream&>;
    template <class T> void operator()(T&& val) const {*os << std::forward<T>(val) << term;}
  };
  constexpr print_fn log_stdout {
    .os = &std::cout,
    .term = std::endl
  };
}
#endif // MISC_UTILS_HPP
