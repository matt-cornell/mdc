#ifndef STRING_HPP
#define STRING_HPP
#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>
namespace mdc {
  namespace {
    const char* copy_string(const std::string& str) {auto ptr = (char*)std::malloc(str.size()); std::strcpy(ptr, str.data()); return ptr;}
    const char* copy_string(const char* str, std::size_t sz) {auto ptr = (char*)std::malloc(sz); std::memcpy(ptr, str, sz); return ptr;}
  }
  class string {
    const char* _data;
    std::size_t _size;
    bool _owning;
  public:
    using iterator = const char*;
    using const_iterator = const char*;
    string() = default;
    string(const string& other) : _data(copy_string(other._data, other._size)), _size(other._size), _owning(true) {}
    string(const char* str, bool own = false) : _data(str), _size(std::strlen(str)), _owning(own) {}
    string(const char* str, std::size_t sz, bool own = false) : _data(str), _size(sz), _owning(own) {}
    string(const std::string& str, bool own = false) : _data(own ? copy_string(str) : str.data()), _size(str.size()), _owning(own) {}
    string(std::string&& str, bool own = true) : _data(own ? copy_string(str) : str.data()), _size(str.size()), _owning(own) {}
    ~string() {if (_owning) delete _data;}
    inline const char* data() const noexcept {return _data;}
    inline const char* c_str() const noexcept {return _data;}
    inline const std::size_t& size() const noexcept {return _size;}
    inline operator std::string_view() const noexcept {return {_data, _size};}
    inline std::string_view as_view() const noexcept {return *this;}
    inline std::string to_string() const {return {_data, _size};}
    inline string get_ref() const noexcept {return {_data, _size, false};}
    inline string get_cpy() const noexcept{return *this;}
    inline iterator begin() noexcept {return _data;}
    inline iterator end() noexcept {return _data + _size;}
    inline const_iterator begin() const noexcept {return _data;}
    inline const_iterator end() const noexcept {return _data + _size;}
    inline string substr(std::size_t start) const {
      if (start >= _size) throw std::out_of_range("");
      return {_data + start, _size - start, false};
    }
    inline string substr(std::size_t start, std::size_t len) const {
      if (start + len >= _size) throw std::out_of_range("");
      return {_data + start, len, false};
    }
    inline const char& operator[](std::size_t idx) const noexcept {return _data[idx];}
    inline const char& at(std::size_t idx) const {
      if (idx >= _size) throw std::out_of_range("");
      return _data[idx];
    }
#define DEF_MO_CMP(OP) bool operator OP(const string& other) const {return as_view() OP other.as_view();}
    DEF_MO_CMP(<)
    DEF_MO_CMP(>)
    DEF_MO_CMP(<=)
    DEF_MO_CMP(>=)
    DEF_MO_CMP(==)
    DEF_MO_CMP(!=)
#undef DEF_MO_CMP
  };
#define DEF_MO_CONCAT(LHS, RHS) inline string operator+(LHS lhs, RHS rhs) { \
    auto size = lhs.size() + rhs.size(); \
    auto data = new char[size]; \
    std::memcpy(data, lhs.data(), lhs.size()); \
    std::memcpy(data + lhs.size(), rhs.data(), rhs.size()); \
    return {data, size, true}; \
  }
  DEF_MO_CONCAT(const string&, const string&)
  DEF_MO_CONCAT(const std::string&, const string&)
  DEF_MO_CONCAT(const string&, const std::string&)
  DEF_MO_CONCAT(const string&, std::string_view)
  DEF_MO_CONCAT(std::string_view, const string&)
  inline string operator+(const string& lhs, const char* rhs) {
    auto len = std::strlen(rhs);
    auto size = lhs.size() + len;
    auto data = new char[size];
    std::memcpy(data, lhs.data(), lhs.size());
    std::memcpy(data + lhs.size(), rhs, len);
    return {data, size, true};
  }
  inline string operator+(const char* lhs, const string& rhs) {
    auto len = std::strlen(lhs);
    auto size = len + rhs.size();
    auto data = new char[size];
    std::memcpy(data, lhs, len);
    std::memcpy(data + len, rhs.data(), rhs.size());
    return {data, size, true};
  }
  inline string operator+(const string& lhs, char rhs) {
    auto size = lhs.size() + 1;
    auto data = new char[size];
    std::memcpy(data, lhs.data(), lhs.size());
    *(data + lhs.size()) = rhs;
    return {data, size, true};
  }
  inline string operator+(char lhs, const string& rhs) {
    auto size = 1 + rhs.size();
    auto data = new char[size];
    *data = lhs;
    std::memcpy(data + 1, rhs.data(), rhs.size());
    return {data, size, true};
  }
  template <std::size_t len> inline string operator+(const string& lhs, const char rhs[len]) {
    auto size = lhs.size() + len;
    auto data = new char[size];
    std::memcpy(data, lhs.data(), lhs.size());
    std::memcpy(data + lhs.size(), rhs, len);
    return {data, size, true};
  }
  template <std::size_t len> inline string operator+(const char lhs[len], const string& rhs) {
    auto size = len + rhs.size();
    auto data = new char[size];
    std::memcpy(data, lhs, len);
    std::memcpy(data + len, rhs.data(), rhs.size());
    return {data, size, true};
  }
#undef DEF_MO_CONCAT
  inline string& operator+=(string& lhs, const string& rhs) {return lhs = lhs + rhs;}
  inline string& operator+=(string& lhs, const std::string& rhs) {return lhs = lhs + rhs;}
  inline string& operator+=(string& lhs, const char* rhs) {return lhs = lhs + rhs;}
  template <std::size_t sz> inline string& operator+=(string& lhs, const char rhs[sz]) {return lhs = lhs + rhs;}
  inline namespace literals {
    inline namespace string_literals {
      inline string operator""_ms(const char* str, unsigned long sz) {return {str, (std::size_t)sz};}
    }
  }
}
namespace std {template <> struct hash<mdc::string> {std::size_t operator()(const mdc::string& str) const {return std::hash<std::string_view>()(str);}};}
#endif // STRING_HPP
