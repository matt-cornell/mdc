#ifndef BORROW_FUNC_HPP
#define BORROW_FUNC_HPP
#include <utility>
namespace mdc {
  template <class T> struct borrow_function;
  template <class T> struct is_borrow_function : std::false_type {};
  template <class T> struct is_borrow_function<borrow_function<T>> : std::true_type {};
  template <class R, class... As> struct borrow_function<R(As...)> {
    void* ptr;
    R(*call)(void*, As...);
    template <class F> borrow_function(F& fn) : ptr((void*)&fn), call([](void* self, As... args)->R {return (*(F*)self)(std::forward<As>(args)...);}) {}
    borrow_function(borrow_function<R(As...)>& fn) : borrow_function((borrow_function<R(As...)> const&)fn) {}
    borrow_function(borrow_function<R(As...)> const&) = default;
    borrow_function& operator=(borrow_function<R(As...)> const&) = default;
    R operator()(As... args) const {return call(ptr, std::forward<As>(args)...);}
  };
  template <class... As> struct borrow_function<void(As...)> {
    void* ptr;
    void(*call)(void*, As...);
    borrow_function(borrow_function<void(As...)>& fn) : borrow_function((borrow_function<void(As...)> const&)fn) {}
    borrow_function(borrow_function<void(As...)> const&) = default;
    template <class F> borrow_function(F& fn) : ptr((void*)&fn), call([](void* self, As... args) {(*(F*)self)(std::forward<As>(args)...);}) {}
    borrow_function& operator=(borrow_function<void(As...)> const&) = default;
    void operator()(As... args) const {call(ptr, std::forward<As>(args)...);}
  };
}
#endif // BORROW_FUNC_HPP
