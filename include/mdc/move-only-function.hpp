#ifndef MOVE_ONLY_FUNCTION_HPP
#define MOVE_ONLY_FUNCTION_HPP
namespace mdc {
  namespace detail {
    template <class R, class... As> struct callable_base {virtual R call(As...) const = 0;};
    template <class F, class R, class... As> struct callable : callable_base<R, As...> {
      [[no_unique_address]] F fn;
      virtual R call(As... args) const {return fn(args...);}
    };
  }
  
}
#endif // MOVE_ONLY_FUNCTION_HPP
