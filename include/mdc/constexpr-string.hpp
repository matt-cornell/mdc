#ifndef CONSTEXPR_STRING_HPP
#define CONSTEXPR_STRING_HPP
#include <utility>
namespace mdc {
  template <char... cs> struct cstring {
    constexpr static char chars = {cs...};
    constexpr static unsigned long size = sizeof...(cs);
    constexpr static char c_str = {cs..., 0};
  };
  namespace detail {
    template <class L, class R> struct concat;
    template <char... L, char... R> struct concat<cstring<L...>, cstring<R...>> {using type = cstring<L..., R...>;};
  }
  template <class L, class R> using concat = typename detail::concat<L, R>::type;
  namespace detail {
    template <class T, unsigned long start, unsigned long end> struct substr;
    template <char C, char... Cs> struct pop_first {using type = cstring<Cs...>;};
    template <char... C, unsigned long S, unsigned long E> struct substr<cstring<C...>, S, E> : substr<typename pop_first<C...>::type, S - 1, E> {};
    template <char C, char... Cs> struct pop_last {using type = cstring<C, pop_last<Cs...>::type>;};
    template <char C1, char C2> struct pop_last<C1, C2> {using type = cstring<C1>;};
    template <char... C, unsigned long E> struct substr<cstring<C...>, 0, E> : substr<typename pop_last<C...>::type, 0, E - 1> {};
    template <char... C> struct substr<cstring<C...>, 0, 0> {using type = cstring<C...>;};
  }
  template <class T, unsigned long start, unsigned long end> using substr = typename detail::substr<T, start, end>::type;
  namespace detail {
    template <long val, char... Cs> struct expand_long : expand_long<val / 10, val % 10 + '0', Cs...> {};
    template <long val> requires (val < 0) struct expand_long<val> : expand_long<-val, '-'> {};
    template <char... Cs> struct expand_long<0, Cs...> {using type = cstring<Cs...>;};
  }
  template <long val> using ltos = typename detail::expand_long<val>::type;
  template <class T, T val> struct to_cstring_selector;
  template <long val> struct to_cstring_selector<long, val> {using type = ltos<val>;};
  template <class T, T val> using to_cstring = typename to_cstring_selector<T, val>::type;
  template <class T> constexpr auto convert_to_cstring(T val) -> to_cstring<T, val> {return {};}
  namespace detail {
    template <unsigned long size, char fill, char... prefix> struct cstring_of_size : cstring_of_size<size - 1, fill, fill, prefix...> {};
    template <char fill, char... str> struct cstring_of_size<0, fill, str...> {using type = cstring<str...>;};
  }
  template <unsigned long size, char fill = 0> using cstring_of_size = typename detail::cstring_of_size<size, fill>::type;
  inline namespace literals {
    inline namespace cstring_literals {
      template <char... Cs> cstring<Cs...> operator""_cs() {return {};}
    }
  }
  namespace detail {
    template <class T, std::size_t... i> constexpr cstring<T::chars()[i]...> builder(std::index_sequence<i...>) {return {};}
    template <class T> T mirror(T);
  }
}
#if !defined(MDC_NO_MACROS) && !defined(MDC_CSTRING_NO_MACROS)
#define MDC_CSTRING(STR) []{struct type {constexpr static decltype(auto) chars() {return STR;}};return builder<type>(std::make_index_sequence<13>{});};
#endif
#endif // CONSTEXPR_STRING_HPP
