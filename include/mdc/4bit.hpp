#ifndef MDC_4BIT_HPP
#define MDC_4BIT_HPP
#include <iostream>
namespace mdc {
  class reader4 {
    char c;
    bool to_read;
    std::istream& in;
  public:
    reader4(std::istream& in_arg) : in(in_arg) {}
    char get4() {
      if (to_read) {
        to_read = false;
        in.get(c);
        return c & 0x0F;
      }
      else {
        to_read = true;
        return c >> 4;
      }
    }
    reader4& get4(char& ch) {
      if (to_read) {
        to_read = false;
        in.get(c);
        ch = c & 0x0F;
      }
      else {
        to_read = true;
        ch = c >> 4;
      }
      return *this;
    }
    reader4& read4(char* buff, std::size_t sz) {
      if (!sz) return *this;
      if (to_read) buff[0] = c >> 4;
      for (std::size_t i = 0; i < sz / 2; ++i) {
        in.get(c);
        buff[i * 2 + to_read] = c & 0x0F;
        buff[i * 2 + to_read + 1] = c >> 4;
      }
      if ((sz - to_read) % 2) {
        in.get(c);
        buff[sz - 1] = c & 0x0F;
      }
      to_read = to_read ^ sz % 2;
      return *this;
    }
    char get8() {
      if (to_read) {
        char ch = c;
        in.get(c);
        return (ch & 0xF0) | (c & 0x0F);
      }
      else return in.get();
    }
    reader4& get8(char& ch) {
      if (to_read) {
        char c_ = c;
        in.get(c);
        ch = (c_ & 0xF0) | (c & 0x0F);
      }
      else in.get(ch);
      return *this;
    }
    reader4& read8(char* buff, std::size_t sz) {for (std::size_t i = 0; i < sz; ++i) buff[i] = get8(); return *this;} // TODO: implement efficiently
    reader4& unget4() {
      if (to_read) to_read = false;
      else {
        in.unget();
        (void)get4();
      }
      return *this;
    }
    reader4& unget8() {
      if (to_read) in.unget();
      else {
        in.unget();
        in.unget();
        (void)get4();
      }
      return *this;
    }
    reader4& peek4(char& ch) {ch = to_read ? in.peek() << 4 : c & 0x0F; return *this;}
    reader4& peek8(char& ch) {ch = to_read ? ((char)in.peek() << 4) | (c & 0x0F) : in.peek(); return *this;}
    char peek4() {return to_read ? in.peek() << 4 : c & 0x0F;}
    char peek8() {return to_read ? ((char)in.peek() << 4) | (c & 0x0F) : in.peek();}
    explicit operator bool() {return static_cast<bool>(in);}
    inline bool eof() const {return in.eof();}
  };
  class writer4 {
    char ch;
    bool to_write;
    std::ostream& out;
  public:
    writer4(std::ostream& out_arg) : out(out_arg) {}
    writer4& put4(char c) {
      if (to_write) {
        to_write = false;
        out.put(ch | (c << 4));
      }
      else {
        to_write = true;
        ch = c & 0x0F;
      }
      return *this;
    }
    writer4& write4(const char* buff, std::size_t sz) {
      if (!sz) return *this;
      if (to_write) out.put(ch | buff[0]);
      for (const char* i = buff + to_write; i != buff + sz; ++i) {
        out.put(*i & 0x0F);
        out.put(*i >> 4);
      }
      if ((sz - to_write) % 2) ch = buff[sz - 1] & 0x0F;
      to_write = to_write ^ sz % 2;
      return *this;
    }
    writer4& put8(char c) {
      if (to_write) {
        out.put((ch & 0x0F) | (c & 0xF0));
        ch = c & 0x0F;
      }
      else out.put(c);
      return *this;
    }
    writer4& write8(const char* buff, std::size_t sz) {for (const char* i = buff; i != buff + sz; ++i) put8(*i); return *this;}
    explicit operator bool() {return static_cast<bool>(out);}
  };
}
#endif // MDC_4BIT_HPP
