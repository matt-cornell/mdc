#ifndef POLYMORPHIC_HPP
#define POLYMORPHIC_HPP
#include <type_traits>
#include <utility>
namespace mdc {
  struct default_allocator {void* operator()(std::size_t size) {return ::operator new(size);}};
  template <class T, class Allocator = default_allocator> struct polymorphic {
    virtual ~polymorphic() = default;
    virtual T* clone() const {
      using T2 = std::remove_cvref_t<decltype (*this)>;
      return static_cast<T*>(new (Allocator()(sizeof (T2))) T2 (*this));
    }
    virtual T* move_to() {
      using T2 = std::remove_cvref_t<decltype (*this)>;
      return static_cast<T*>(new (Allocator()(sizeof (T2))) T2 (std::move(*this)));
    }
  };
}
#endif // POLYMORPHIC_COPY_HPP
