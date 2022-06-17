#ifndef pre_hashed_HPP
#define pre_hashed_HPP
#include <utility>
#include <concepts>
#include <bits/functional_hash.h>
namespace mdc {
  template <class T, class Hasher = std::hash<T>> struct pre_hashed {
    T val;
    std::size_t hash;
    pre_hashed() = default;
    pre_hashed(const T& val_arg) : val(val_arg), hash(Hasher{}(val_arg)) {}
    pre_hashed(const T& val_arg, std::size_t hash_arg) : val(val_arg), hash(hash_arg) {}
    pre_hashed(T&& val_arg) : val(std::move(val_arg)), hash(Hasher{}(val_arg)) {}
    pre_hashed(T&& val_arg, std::size_t hash_arg) : val(std::move(val_arg)), hash(hash_arg) {}
    template <std::convertible_to<T> T2> pre_hashed(T2&& arg) : val(std::forward<T2>(arg)), hash(Hasher{}(val)) {}
    template <class H = Hasher> pre_hashed<T, Hasher>& rehash(H&& hasher = H()) {hash = hasher(val); return *this;}
  };
#define DEF_OP(OP) template <class T, class Hasher> decltype(std::declval<T>() OP std::declval<T>()) operator OP(const pre_hashed<T, Hasher>& lhs, const pre_hashed<T, Hasher>& rhs) {return lhs.val OP rhs.val;}
  DEF_OP(<)
  DEF_OP(>)
  DEF_OP(<=)
  DEF_OP(>=)
  DEF_OP(==)
  DEF_OP(!=)
#undef DEF_OP
}
namespace std {template <class T, class Hasher> struct hash<mdc::pre_hashed<T, Hasher>> {inline std::size_t operator()(const mdc::pre_hashed<T, Hasher>& in) const {return in.hash;}};}
#endif
