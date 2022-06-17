#ifndef IDENTITY_ASSOCIATIVE_HPP
#define IDENTITY_ASSOCIATIVE_HPP
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
namespace mdc {
  template <class T> struct identity_hash {std::size_t operator()(const T& val) const noexcept {return (std::size_t)&val;}};
  template <class T> struct identity_equal {bool operator()(const T& lhs, const T& rhs) const noexcept {return &lhs == &rhs;}};
  template <class T> struct identity_compare {bool operator()(const T& lhs, const T& rhs) const noexcept {return &lhs < &rhs;}};
  template <class T, class Alloc = std::allocator<T>> using iset = std::set<T, identity_compare<T>, Alloc>;
  template <class K, class V, class Alloc = std::allocator<std::pair<const K, V>>> using imap = std::map<K, V, identity_compare<K>, Alloc>;
  template <class T, class Alloc = std::allocator<T>> using unordered_iset = std::unordered_set<T, identity_hash<T>, identity_equal<T>, Alloc>;
  template <class K, class V, class Alloc = std::allocator<std::pair<const K, V>>> using unordered_imap = std::unordered_map<K, V, identity_hash<K>, identity_equal<K>, Alloc>;
}
#endif // IDENTITY_ASSOCIATIVE_HPP
