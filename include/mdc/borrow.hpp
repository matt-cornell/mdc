#ifndef BORROW_HPP
#define BORROW_HPP
#include <cstring>
#include <utility>
#include <type_traits>
#ifndef __clang__
#define __has_builtin(T) 0
#endif
namespace mdc {
  template <class T> class alignas(T) borrow {
    char8_t data[sizeof(T)];
  public:
    borrow() = delete;
#if __has_builtin(__builtin_memcpy)
    constexpr borrow(const T& other) {__builtin_memcpy(data, &other, sizeof(T));}
#else
    borrow(const T& other) {std::memcpy(data, &other, sizeof(T));}
#endif
    template <class R, class... As, class... Args> R call(R(T::*fn)(As...) const, Args&&... args) const requires (!std::is_fundamental_v<T>) {return (reinterpret_cast<T const*>(this)->*fn)(std::forward<Args>(args)...);}
    template <class U> U const& attr(const U T::* attr) const {return reinterpret_cast<T const*>(this)->*attr;}
    operator T const&() const {return *reinterpret_cast<T const*>(this);}
    T const& get() const {return *reinterpret_cast<T const*>(this);}
  };
}
#endif // BORROW_HPP
