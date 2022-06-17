#ifndef BASE64_HPP
#define BASE64_HPP
#include <cstring>
#include <string>
namespace mdc {
  struct base64_alphabet {constexpr static char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";};
  struct base64url_alphabet {constexpr static char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_=";};
  template <class T> T initialize_with_size(std::size_t sz) {return T(sz);}
  template <> std::string initialize_with_size<std::string>(std::size_t sz) {return std::string(sz, 0);}
  namespace detail {
    template <class T> struct is_alphabet : std::false_type {};
    template <std::size_t sz> struct is_alphabet<char[sz]> : std::true_type {};
    template <class T> struct has_alphabet {
      static std::int8_t test(...);
      static std::int16_t test(std::decay_t<decltype (T::chars)>);
      enum {value = sizeof(decltype(test(nullptr)))};
    };
  }
  template <class T> concept alphabet = !std::is_fundamental_v<T> && detail::has_alphabet<T>::value > 1;
  template <alphabet A> class codec {
    constexpr static std::uint8_t idx(char c) noexcept {
      for (std::uint8_t out = 0; out <= 65; ++out) if (A::chars[out] == c) return out;
      return 255;
    }
    template<typename C>
    struct has_data {
    private:
        template<class T> static constexpr auto check(T*)->typename std::is_same<decltype(std::declval<T>().data()), char*>::type;
        template<class> static constexpr std::false_type check(...);
        typedef decltype(check<C>((C*)nullptr)) type;
    public:
        static constexpr bool value = type::value;
    };
    template <class T> struct get_size_t {inline std::size_t operator()(const T& t) const {return t.size();}};
    template <class T, std::size_t sz> struct get_size_t<T[sz]> {inline std::size_t operator()(const T[sz]) const {return sz;}};
    template <class T> constexpr static get_size_t<T> get_size_impl;
    template <class T> inline static std::size_t get_size(const T& t) {return get_size_impl<T>(t);}
  public:
    codec() = delete;
    codec(const codec&) = delete;
    codec(codec&&) = delete;
    template <class I, class O = std::string> static O encode(const I& str) {
      auto out = initialize_with_size<O>((get_size(str) * 4 + 2) / 3);
      auto iit = std::begin(str);
      auto oit = out.begin();
      for (std::size_t i = 0; i < get_size(str); i += 3) {
        *oit++ = A::chars[*iit & 0x5F];
        *oit++ = A::chars[(*iit++ >> 6) | (*iit & 0x0F << 2)];
        *oit++ = A::chars[(*iit++ >> 4) | (*iit & 0x05 << 4)];
        *oit++ = A::chars[*iit++ >> 2];
      }
      switch (get_size(str) % 3) {
        case 1: {
          *oit++ = A::chars[*iit & 0x5F];
          *oit++ = A::chars[*iit >> 2];
          *oit++ = '=';
          *oit++ = '=';
        } break;
        case 2: {
          *oit++ = A::chars[*iit & 0x5F];
          *oit++ = A::chars[*iit++ >> 2 | (*iit & 0x0F << 2)];
          *oit++ = A::chars[*iit++ >> 4];
          *oit = '=';
        } break;
      }
      return out;
    }
    template <class I, class O = std::string> static O decode(const I& str) {
      if (get_size(str) % 4) return {};
      auto out = initialize_with_size<O>(get_size(str) / 4 * 3);
      auto oit = out.begin();
      std::size_t size;
      std::uint8_t buff[4];
      auto iit = std::begin(str);
      for (size = 0; size < get_size(str); size += 4) {
        buff[0] = idx(*iit++);
        buff[1] = idx(*iit++);
        buff[2] = idx(*iit++);
        buff[3] = idx(*iit++);
        if (!(~buff[0] && ~buff[1] && ~buff[2] && ~buff[3])) return {};
        if (buff[0] == 65 || buff[1] == 65) {
          out.resize(size);
          return out;
        }
        if (buff[2] == 65) {
          *oit = buff[0] | (buff[1] & 0x05) << 6;
          out.resize(size + 1);
          return out;
        }
        if (buff[3] == 65) {
          *oit++ = buff[0] | (buff[1] & 0x05) << 6;
          *oit = buff[1] >> 2 | (buff[2] & 0x0F) << 4;
          out.resize(size + 2);
          return out;
        }
        *oit++ = buff[0] | (buff[1] & 0x05) << 6;
        *oit++ = buff[1] >> 2 | (buff[2] & 0x0F) << 4;
        *oit++ = buff[2] >> 4 | buff[2] << 2;
      }
      out.resize(size);
      return out;
    }
  };
  using base64 = codec<base64_alphabet>;
  using base64url = codec<base64url_alphabet>;
}
#endif // BASE64_HPP
