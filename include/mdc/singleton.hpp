#ifndef SINGLETON_HPP
#define SINGLETON_HPP
#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
namespace mdc {
  template <class T> struct singleton;
  namespace detail {
    template <class T> struct singleton_detector : std::false_type {};
    template <class T> struct singleton_detector<singleton<T>> : std::true_type {using type = T;};
    template <class T> concept is_singleton = singleton_detector<T>::value;
    template <class T> concept singleton_pointer = requires (T const& val) {{*val}->is_singleton;};
    template <class T> using singleton_t = typename singleton_detector<std::remove_cvref_t<T>>::type;
    struct elem_hash {
      using is_transparent = void;
      template <class T> std::size_t operator()(T const& val) const {return std::hash<std::decay_t<decltype(*val)>>{}(*val);}
      template <singleton_pointer T> std::size_t operator()(T const& val) const {return std::hash<singleton_t<T>>{}(val);}
    };
    struct elem_eq {
      using is_transparent = void;
      template <class L, class R> bool operator()(L const& lhs, R const& rhs) const {return *lhs == *rhs;}
    };
    struct elem_cmp {
      using is_transparent = void;
      template <class L, class R> bool operator()(L const& lhs, R const& rhs) const {return *lhs < *rhs;}
    };
    template <class T> struct optional_deleter {
      bool del;
      optional_deleter(bool del) : del(del) {}
      void operator()(T const* ptr) {if (del) delete ptr;}
      operator std::default_delete<T>() const {return {};}
    };
    template <class T, class K> concept heterogenous_lookup = requires (T const& set, K const& key) {{**set.find(key)};};
  }
  template <class T> struct singleton : T {
    inline operator T const&() const {return static_cast<T const&>(*this);}
  private:
    singleton(singleton&&) = default;
    using T::T;
    template <class, class> friend class singleton_manager;
    template <class, class, class> friend class static_singleton_manager;
    template <class, class> friend class singleton_type;
    template <class, class> friend class rc_singleton_type;
  };
  template <class T> requires std::is_fundamental_v<T> struct alignas(T) singleton<T> {
    inline operator T const&() const {return reinterpret_cast<T const&>(*this);}
  private:
    singleton(singleton&&) = default;
    char8_t memory[sizeof(T)];
#ifndef __clang__
#define __has_builtin(T) 0
#endif
#if __has_builtin(__builtin_memcpy)
    constexpr singleton(T val) {__builtin_memcpy(memory, &val, sizeof(T));}
#else
    singleton(T val) {std::memcpy(memory, &val, sizeof(T));}
#endif
    singleton() = default;
    template <class, class> friend class singleton_manager;
    template <class, class, class> friend class static_singleton_manager;
    template <class, class> friend class singleton_type;
    template <class, class> friend class rc_singleton_type;
  };
}
namespace mdc {
  template <class T> using ptr_type = std::unique_ptr<singleton<T> const, detail::optional_deleter<singleton<T>>>;
  template <class T> using elem_uset = std::unordered_set<ptr_type<T>, detail::elem_hash, detail::elem_eq>;
  template <class T> using elem_set = std::set<ptr_type<T>, detail::elem_cmp>;
  template <class T> using elem_umap = std::unordered_map<ptr_type<T>, std::size_t, detail::elem_hash, detail::elem_eq>;
  template <class T> using elem_map = std::map<ptr_type<T>, std::size_t, detail::elem_cmp>;
  template <class T, class C = elem_uset<T>> class singleton_manager {
    C set;
  public:
    template <class K> inline singleton<T> const& get(K const& key) {
      if constexpr(detail::heterogenous_lookup<C, K const*>) {
        auto it = set.find(&key);
        if (it == set.end()) it = set.insert(ptr_type<T>(new singleton<T>(key), true)).first;
        return **it;
      }
      else {
        singleton<T> s(key);
        auto it = set.find(ptr_type<T>(&s, false));
        if (it == set.end()) it = set.insert(ptr_type<T>(new singleton<T>(std::move(s)), true)).first;
        return **it;
      }
    }
    template <class K> inline void release(K const& key) {
      if constexpr(detail::heterogenous_lookup<C, K const*>) set.erase(&key);
      else {
        singleton<T> s(key);
        set.find(ptr_type<T>(&s, false));
      }
    }
  };
  template <class T, class C = elem_uset<T>, class tag = void> class static_singleton_manager {
    inline static C set;
  public:
    template <class K> inline static singleton<T> const& get(K const& key) {
      if constexpr(detail::heterogenous_lookup<C, K const*>) {
        auto it = set.find(&key);
        if (it == set.end()) it = set.insert(ptr_type<T>(new singleton<T>(key), true)).first;
        return **it;
      }
      else {
        singleton<T> s(key);
        auto it = set.find(ptr_type<T>(&s, false));
        if (it == set.end()) it = set.insert(ptr_type<T>(new singleton<T>(std::move(s)), true)).first;
        return **it;
      }
    }
    template <class K> inline static void release(K const& key) {
      if constexpr(detail::heterogenous_lookup<C, K const*>) set.erase(&key);
      else {
        singleton<T> s(key);
        set.find(ptr_type<T>(&s, false));
      }
    }
  };
  template <class T> struct singleton_key {
    T const* ptr;
    singleton_key(T const& val) : ptr(static_singleton_manager<T>::get(val)) {}
    singleton_key(singleton<T> const& val) : ptr(static_cast<T const*>(&val)) {}
    bool operator==(singleton_key<T> other) const {return ptr == other.ptr;}
    bool operator!=(singleton_key<T> other) const {return ptr != other.ptr;}
    T const& operator*() const {return *ptr;}
    T const* operator->() const {return ptr;}
  };
  template <class T> struct val_cmp {bool operator()(singleton_key<T> lhs, singleton_key<T> rhs) const {return *lhs < *rhs;}};
  template <class T> struct ptr_cmp {bool operator()(singleton_key<T> lhs, singleton_key<T> rhs) const {return lhs.ptr < rhs.ptr;}};
  inline namespace literals {
    inline namespace singleton_literals {
      inline singleton<unsigned long long> const& operator""_s(unsigned long long val) {return static_singleton_manager<unsigned long long>::get(val);}
      inline singleton<long double> const& operator""_s(long double val) {return static_singleton_manager<long double>::get(val);}
      inline singleton<std::string> const& operator""_s(const char* val, std::size_t len) {return static_singleton_manager<std::string>::get(std::string_view(val, len));}
      inline singleton<unsigned long long> const& operator""_si(unsigned long long val) {return static_singleton_manager<unsigned long long>::get(val);}
      inline singleton<long double> const& operator""_sf(long double val) {return static_singleton_manager<long double>::get(val);}
      inline singleton<std::string> const& operator""_ss(const char* val, std::size_t len) {return static_singleton_manager<std::string>::get(std::string_view(val, len));}
      inline singleton<unsigned long long> const* operator""_sp(unsigned long long val) {return &static_singleton_manager<unsigned long long>::get(val);}
      inline singleton<long double> const* operator""_sp(long double val) {return &static_singleton_manager<long double>::get(val);}
      inline singleton<std::string> const* operator""_sp(const char* val, std::size_t len) {return &static_singleton_manager<std::string>::get(std::string_view(val, len));}
      inline singleton<unsigned long long> const* operator""_sip(unsigned long long val) {return &static_singleton_manager<unsigned long long>::get(val);}
      inline singleton<long double> const* operator""_sfp(long double val) {return &static_singleton_manager<long double>::get(val);}
      inline singleton<std::string> const* operator""_ssp(const char* val, std::size_t len) {return &static_singleton_manager<std::string>::get(std::string_view(val, len));}
    }
  }
  template <class T> using ssm = static_singleton_manager<T>;
  using singleton_string = singleton<std::string>;
  using ss_key = singleton_key<std::string>;
  using ss_ptr = singleton_string const*;
  using ss_ref = singleton_string const&;
  template <class T> inline singleton<T> const& get_singleton(T const& val) {return static_singleton_manager<T>::get(val);}
}
namespace std {
  template <class T> struct hash<mdc::singleton_key<T>> {std::size_t operator()(mdc::singleton_key<T> const& val) const {return std::hash<T const*>{}(val.ptr);}};
  template <class T> struct hash<mdc::singleton<T>> {std::size_t operator()(mdc::singleton<T> const& val) const {return std::hash<T>{}(val.operator const T &());}};
}
#endif // SINGLETON_HPP
