#ifndef MDC_ITERATOR_HPP
#define MDC_ITERATOR_HPP
#include <typeindex>
namespace mdc {
  template <class T, class R = T&> class forward_iterator {
    void(*it_inc)(void*);
    bool(*it_cmp)(void*, void*);
    R(*it_deref)(void*);
    void*(*clone)(void*);
    void(*destroy)(void*);
    void* data;
    std::type_index type_;
    public:
    template <class I> forward_iterator(I&& it) :
      it_inc([](void* ptr) {++*(I*)ptr;}),
      it_cmp([](void* lhs, void* rhs) {return *(I*)lhs == *(I*)rhs;}),
      it_deref([](void* ptr)-> R {return **(I*)ptr;}),
      clone([](void* ptr) {return new I(ptr);}),
      destroy([](void* ptr) {delete (I*)ptr;}),
      data(new I(std::move(it))),
      type(typeid(I)) {}
    template <class I> forward_iterator(I const& it) :
      it_inc([](void* ptr) {++*(I*)ptr;}),
      it_cmp([](void* lhs, void* rhs) {return *(I*)lhs == *(I*)rhs;}),
      it_deref([](void* ptr)-> R {return **(I*)ptr;}),
      clone([](void* ptr) {return new I(ptr);}),
      destroy([](void* ptr) {delete (I*)ptr;}),
      data(new I(it)),
      type_(typeid(I)) {}
    forward_iterator(forward_iterator<T> const& other) :
      it_inc(other.it_inc),
      it_cmp(other.it_cmp),
      it_deref(other.it_deref),
      clone(other.clone),
      destroy(other.destroy),
      data(clone(other.data)),
      type_(typeid(I)) {}
    forward_iterator(forward_iterator<T>&& other) :
      it_inc(other.it_inc),
      it_cmp(other.it_cmp),
      it_deref(other.it_deref),
      clone(other.clone),
      destroy(other.destroy),
      data(other.data),
      type_(other.type_) {other.data = nullptr;}
    ~forward_iterator() {if (data) destroy(data);}
    forward_iterator<T>& operator=(forward_iterator<T> const& other) {
      if (other.data == data) return *this;
      destroy(data);
      it_inc = other.it_inc;
      it_cmp = other.it_cmp;
      it_deref = other.it_deref;
      clone = other.clone;
      destroy = other.destroy;
      data = clone(other.data);
      type_ = other.type_;
      return *this;
    }
    forward_iterator<T>& operator=(forward_iterator<T>&& other) {
      if (other.data == data) return *this;
      destroy(data);
      it_inc = other.it_inc;
      it_cmp = other.it_cmp;
      it_deref = other.it_deref;
      clone = other.clone;
      destroy = other.destroy;
      data = other.data;
      other.data = nullptr;
      type_ = other.type_;
      return *this;
    }
    forward_iterator<T>& operator++() {
      it_inc(data);
      return *this;
    }
    forward_iterator<T> operator++(int) {
      auto out = *this;
      it_inc(data);
      return out;
    }
    R operator*() const {return it_deref(data);}
    bool operator==(forward_iterator<T, R> const& other) const {return type_ == other.type_ && it_cmp(data, other.data);}
  };
  template <class T, class R> class bidirectional_iterator : public forward_iterator<T, R> {
    void(*it_dec)(void*);
    public:
    template <class I> bidirectional_iterator(I const& it) :
      forward_iterator<T, R>(it),
      it_dec([](void* ptr) {--(I*)ptr;}), {}
    template <class I> bidirectional_iterator(I&& it) :
      forward_iterator<T, R>(std::move(it)),
      it_dec([](void* ptr) {--(I*)ptr;}), {}
    bidirectional_iterator<T>& operator++() {
      it_inc(data);
      return *this;
    }
    bidirectional_iterator<T> operator++(int) {
      auto out = *this;
      it_inc(data);
      return out;
    }
    bidirectional_iterator<T>& operator--() {
      it_dec(data);
      return *this;
    }
    bidirectional_iterator<T> operator--(int) {
      auto out = *this;
      it_dec(data);
      return out;
    }
  };
  template <class T, class R = T&> struct forward_range {
    
  };
}
#endif