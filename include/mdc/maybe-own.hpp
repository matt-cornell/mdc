#ifndef MAYBE_OWN_HPP
#define MAYBE_OWN_HPP
#include <memory>
namespace mdc {
  namespace detail {
    template <class T> struct maybe_own {
      virtual T& operator*() = 0;
      virtual T* operator->() = 0;
    };
    template <class T> struct owning : maybe_own<T> {
      std::unique_ptr<T> ptr;
      virtual T& operator*() {return *ptr;}
      virtual T* operator->() {return ptr.get();}
    };
    template <class T> struct non_owning : maybe_own<T> {
      T& val;
      virtual T& operator*() {return val;}
      virtual T* operator->() {return &val;}
    };
    template <class T> inline owning<T> make_maybe_own(T&& val) {return {new std::remove_cvref_t<T>(std::move(val))};}
    template <class T> inline non_owning<T> make_maybe_own(T& val) {return {val};}
  }
  template <class T> using maybe_owning = std::unique_ptr<detail::maybe_own<T>>;
  template <class T> inline maybe_owning<T> make_maybe_own(T&& val) {return std::make_unique<detail::owning>(new std::remove_cvref_t<T>(std::move(val)));}
  template <class T> inline maybe_owning<T> make_maybe_own(T& val) {return std::make_unique<detail::non_owning>(val);}
}
#endif // MAYBE_OWN_HPP
