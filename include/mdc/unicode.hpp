#ifndef UNICODE_HPP
#define UNICODE_HPP
#include <bit>
#include <iostream>
#include <iomanip>
#include <optional>
#include <string>
namespace mdc {
  namespace utf {
    template <class I> inline std::optional<char32_t> step(I& it) noexcept(noexcept(*it++)) {
      switch (std::countl_one((unsigned char)*it)) {
        case 0: return char32_t(*it++); break;
        case 2: {
          char32_t val = (*it++ & 0x3F) << 6;
          if ((*it & 0xC0) ^ 0x80) return std::nullopt;
          val |= (*it++ & 0x3F);
          return val;
        } break;
        case 3: {
          char32_t val = (*it++ & 0x1F) << 12;
          if ((*it & 0xC0) ^ 0x80) return std::nullopt;
          val |= (*it++ & 0x3F) << 6;
          if ((*it & 0xC0) ^ 0x80) return std::nullopt;
          val |= (*it++ & 0x3F);
          return val;
        } break;
        case 4: {
          char32_t val = (*it++ & 0x0F) << 18;
          if ((*it & 0xC0) ^ 0x80) return std::nullopt;
          val |= (*it++ & 0x3F) << 12;
          if ((*it & 0xC0) ^ 0x80) return std::nullopt;
          val |= (*it++ & 0x3F) << 6;
          if ((*it & 0xC0) ^ 0x80) return std::nullopt;
          val |= (*it++ & 0x3F);
          return val;
        } break;
        default: return std::nullopt;
      }
    }
    template <class I> inline bool step(I& it, I& end, char32_t& c) noexcept(noexcept(*it++)) {
      if (it == end) return false;
      switch (std::countl_one((unsigned char)*it)) {
        case 0: c = char32_t(*it++); return true; break;
        case 2: {
          char32_t val = (*it++ & 0x3F) << 6;
          if (it == end) return false;
          if ((*it & 0xC0) ^ 0x80) return false;
          val |= (*it++ & 0x3F);
          if (it == end) return false;
          return val;
        } break;
        case 3: {
          char32_t val = (*it++ & 0x1F) << 12;
          if (it == end) return false;
          if ((*it & 0xC0) ^ 0x80) return false;
          val |= (*it++ & 0x3F) << 6;
          if (it == end) return false;
          if ((*it & 0xC0) ^ 0x80) return false;
          val |= (*it++ & 0x3F);
          return val;
        } break;
        case 4: {
          char32_t val = (*it++ & 0x0F) << 18;
          if (it == end) return false;
          if ((*it & 0xC0) ^ 0x80) return false;
          val |= (*it++ & 0x3F) << 12;
          if (it == end) return false;
          if ((*it & 0xC0) ^ 0x80) return false;
          val |= (*it++ & 0x3F) << 6;
          if (it == end) return false;
          if ((*it & 0xC0) ^ 0x80) return false;
          val |= (*it++ & 0x3F);
          return val;
        } break;
        default: return false;
      }
    }
    template <class I> inline bool append(char32_t val, I it) noexcept(noexcept(*it++)) {
      if (val < 0x80) {
        *it++ = val & 0xFF;
        return true;
      }
      else if (val < 0x800) {
        *it++ = val >> 6 | 0xC0;
        *it++ = (val & 0x3F) | 0x80;
        return true;
      }
      else if (val < 0x10000) {
        *it++ = val >> 12 | 0xE0;
        *it++ = ((val >> 6) & 0x3F) | 0x80;
        *it++ = (val & 0x3F) | 0x80;
        return true;
      }
      else if (val < 0x110000) {
        *it++ = val >> 18 | 0xF0;
        *it++ = ((val >> 12) & 0x3F) | 0x80;
        *it++ = ((val >> 6) & 0x3F) | 0x80;
        *it++ = (val & 0x3F) | 0x80;
      }
      else return false;
    }
    enum character : char32_t;
    enum codepoint : char32_t;
    inline std::ostream& operator<<(std::ostream& os, character c) {
      if (c < 0x80) {
        os << char(c & 0xFF);
      }
      else if (c < 0x800) {
        os << char(c >> 6 | 0xC0);
        os << char((c & 0x3F) | 0x80);
      }
      else if (c < 0x10000) {
        os << char(c >> 12 | 0xE0);
        os << char(((c >> 6) & 0x3F) | 0x80);
        os << char((c & 0x3F) | 0x80);
      }
      else if (c < 0x110000) {
        os << char(c >> 18 | 0xF0);
        os << char(((c >> 12) & 0x3F) | 0x80);
        os << char(((c >> 6) & 0x3F) | 0x80);
        os << char((c & 0x3F) | 0x80);
      }
      return os;
    }
    inline std::ostream& operator<<(std::ostream& os, codepoint c) {
      os << "U+" << std::hex;
      if (c < 0x100) os << std::setw(2) << std::setfill('0');
      else if (c < 0x10000) os << std::setw(4) << std::setfill('0');
      return os << int32_t(c);
    }
  }
}
#endif // UNICODE_HPP
