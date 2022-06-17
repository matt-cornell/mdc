#ifndef BOX_PTR_HPP
#define BOX_PTR_HPP
namespace mdc {
  template <class T> struct box_ptr {
    T* ptr;
    box_ptr(T* ptr) : ptr(ptr) {}
    box_ptr(box_ptr<T> const& other) : ptr(new T(*other.ptr)) {}
    ~box_ptr() {if (ptr) delete ptr;}
    box_ptr& operator=(box_ptr<T> const& other) {
      ptr = new T(*other.ptr);
      return *this;
    }
    T& operator*() const {return *ptr;}
    T* operator->() const {return ptr;}
    bool operator==(box_ptr<T> const& other) const {return ptr == other.ptr;}
  };
}
#endif // BOX_PTR_HPP
