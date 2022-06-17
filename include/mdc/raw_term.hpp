#ifndef RAW_TERM_HPP
#define RAW_TERM_HPP
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
namespace mdc {
	struct termios default_term;
	inline void disable_raw() {
		tcsetattr(fileno(stdin), TCSAFLUSH, &default_term);
	}
	inline void enable_raw() {
		tcgetattr(fileno(stdin), &default_term);
		std::atexit(disable_raw);
		struct termios raw = default_term;
		raw.c_lflag &= ~(ECHO | ICANON);
		tcsetattr(fileno(stdin), TCSAFLUSH, &raw);
	}
	constexpr struct raw_state_t {raw_state_t const& operator=(bool state) const {if (state) enable_raw(); else disable_raw(); return *this;}} raw_state;
}
#endif // RAW_TERM_HPP
