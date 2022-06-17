#ifndef RANGES_HPP
#define RANGES_HPP
#include <deque>
#include <iterator>
#include <numeric>
#include <optional>
#include <boost/range/iterator_range.hpp>
namespace mdc {
  template <class O, class I> O make_range(const I& in) {return {begin(in), end(in)};}
  template <template <class...> class O, class I> O<decltype(*begin(std::declval<I>()))> make_range(const I& in) {return {begin(in), end(in)};}
  template <template <class...> class O, class I, std::size_t sz> O<I> make_range(const I (&in)[sz]) {return {in, in + sz};}
  template <class It> struct sentinel_iterator {
    std::optional<It> value;
    It end;
    typename std::iterator_traits<It>::reference operator*() const {
      if (value) return **value;
      else throw std::out_of_range("Iterator is past the end");
    }
    typename std::iterator_traits<It>::pointer operator->() const {
      if (value) {
        if constexpr (std::is_pointer_v<It>) return *value;
        else return &**value;
      }
      else throw std::out_of_range("Iterator is past the end");
    }
    sentinel_iterator<It>& operator++() {
      if (value) ++*value;
      return *this;
    }
    sentinel_iterator<It>& operator++(int) {
      auto out = *this;
      if (value) *value++;
      return out;
    }
    sentinel_iterator<It>& operator--() requires(std::derived_from<typename std::iterator_traits<It>::iterator_category, std::bidirectional_iterator_tag>) {
      if (value) --*value;
      return *this;
    }
    sentinel_iterator<It>& operator--(int) requires(std::derived_from<typename std::iterator_traits<It>::iterator_category, std::bidirectional_iterator_tag>) {
      auto out = *this;
      if (value) *value--;
      return out;
    }
    sentinel_iterator<It>& operator+=(const typename std::iterator_traits<It>::difference_type& other) requires(std::derived_from<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>) {if (value) *value += other;}
    sentinel_iterator<It>& operator-=(const typename std::iterator_traits<It>::difference_type& other) requires(std::derived_from<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>) {if (value) *value += other;}
    typename std::iterator_traits<It>::reference operator[](const typename std::iterator_traits<It>::difference_type& idx) const requires(std::derived_from<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>) {
      if (value) return (*value)[idx];
      else throw std::out_of_range("Iterator is past the end");
    }
  };
  template <class It> typename std::iterator_traits<It>::difference_type operator-(const sentinel_iterator<It>& lhs, const sentinel_iterator<It>& rhs) requires(std::derived_from<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>) {return lhs.value && rhs.value ? typename std::iterator_traits<It>::difference_type{*lhs.value - *rhs.value} : typename std::iterator_traits<It>::difference_type{};}
  template <class It> sentinel_iterator<It> operator+(const sentinel_iterator<It>& lhs, const typename std::iterator_traits<It>::difference_type& rhs) requires(std::derived_from<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>) {return lhs.value && rhs.value ? sentinel_iterator<It>{*lhs.value + rhs} : sentinel_iterator<It>{};}
  template <class It> sentinel_iterator<It> operator+(const typename std::iterator_traits<It>::difference_type& lhs, const sentinel_iterator<It>& rhs) requires(std::derived_from<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>) {return lhs.value && rhs.value ? sentinel_iterator<It>{lhs + *rhs.value} : sentinel_iterator<It>{};}
  template <class It> sentinel_iterator<It> operator-(const sentinel_iterator<It>& lhs, const typename std::iterator_traits<It>::difference_type& rhs) requires(std::derived_from<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>) {return lhs.value && rhs.value ? sentinel_iterator<It>{*lhs.value - rhs} : sentinel_iterator<It>{};}
  template <class It> sentinel_iterator<It> operator-(const typename std::iterator_traits<It>::difference_type& lhs, const sentinel_iterator<It>& rhs) requires(std::derived_from<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>) {return lhs.value && rhs.value ? sentinel_iterator<It>{lhs - *rhs.value} : sentinel_iterator<It>{};}
  template <class It> bool operator<(const sentinel_iterator<It>& lhs, const sentinel_iterator<It>& rhs) requires(std::derived_from<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>) {return lhs.value && rhs.value && *lhs.value < *rhs.value;}
  template <class It> bool operator>(const sentinel_iterator<It>& lhs, const sentinel_iterator<It>& rhs) requires(std::derived_from<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>) {return lhs.value && rhs.value && *lhs.value > *rhs.value;}
  template <class It> bool operator<=(const sentinel_iterator<It>& lhs, const sentinel_iterator<It>& rhs) requires(std::derived_from<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>) {return lhs.value && rhs.value && *lhs.value <= *rhs.value;}
  template <class It> bool operator>=(const sentinel_iterator<It>& lhs, const sentinel_iterator<It>& rhs) requires(std::derived_from<typename std::iterator_traits<It>::iterator_category, std::random_access_iterator_tag>) {return lhs.value && rhs.value && *lhs.value >= *rhs.value;}
  template <class It> bool operator==(const sentinel_iterator<It>& lhs, const sentinel_iterator<It>& rhs) {
    switch ((bool)lhs.value << 1 | (bool)rhs.value) {
      case 0:
        return true;
        break;
      case 1:
        return *rhs.value == rhs.end;
        break;
      case 2:
        return *lhs.value == lhs.end;
      case 3:
        return *lhs.value == *rhs.value;
        break;
    }
  }
  template <class It> bool operator!=(const sentinel_iterator<It>& lhs, const sentinel_iterator<It>& rhs) {
    switch ((bool)lhs.value << 1 | (bool)rhs.value) {
      case 0:
        return false;
        break;
      case 1:
        return *rhs.value != rhs.end;
        break;
      case 2:
        return *lhs.value != lhs.end;
      case 3:
        return *lhs.value != *rhs.value;
        break;
    }
  }
  template <class R> sentinel_iterator<decltype(begin(std::declval<R&>()))> sentinel(R& in) {return {{begin(in)}, end(in)};}
  template <class R> sentinel_iterator<decltype(begin(std::declval<const R&>()))> sentinel(const R& in) {return {{begin(in)}, end(in)};}
  namespace adaptors {
    template <class R> using range_value_t = std::remove_cvref_t<decltype(*std::declval<R>().begin())>;
    template <class T> struct collect {
      template <class R> T operator()(const R& r) {return T(begin(r), end(r));}
      constexpr const collect& operator()() const {return *this;}
    };
    template <class T> constexpr collect<T> collected;
    template <class R, class T> T operator|(const R& r, collect<T>) {return T(begin(r), end(r));}
    namespace impl {
      template <class I, class F> struct fm_it {
        using iterator_category = std::forward_iterator_tag;
        using reference = std::invoke_result_t<F(typename std::iterator_traits<I>::reference)>;
        using value_type = std::remove_reference_t<reference>;
        using pointer = value_type*;
        F fn;
        I it;
        decltype (auto) operator*() const {
          if (decltype(fn(*it)) out = fn(*it)) return *out;
          else {
            ++it;
            return **this;
          }
        }
        decltype (auto) operator->() const {return &**this;}
        template <class F2> bool operator==(const fm_it<I, F2>& other) const {return it == other.it;}
        template <class F2> bool operator!=(const fm_it<I, F2>& other) const {return it != other.it;}
        operator fm_it<I, const F>() const requires (!std::is_const_v<F>) {return {fn, it};}
        fm_it<I, F>& operator++() {++it; return *this;}
        fm_it<I, F> operator++(int) {fm_it<I, F> self = *this; ++it; return self;}
      };
    }
    template <class R, class F> struct filter_map {
      [[no_unique_address]] F fn;
      using iterator = impl::fm_it<decltype(begin(std::declval<R&>())), F&>;
      using const_iterator = impl::fm_it<decltype(begin(std::declval<const R&>())), const F&>;
      R range;
      impl::fm_it<typename R::iterator, F&> begin() {return {fn, range.begin()};}
      impl::fm_it<typename R::iterator, F&> end() {return {fn, range.end()};}
      impl::fm_it<typename R::const_iterator, F&> begin() const {return {fn, range.begin()};}
      impl::fm_it<typename R::const_iterator, F&> end() const {return {fn, range.end()};}
    };
    template <class F> struct filter_mapped {
      F&& fn;
      filter_mapped(F&& fn) : fn(fn) {}
    };
    template <class R, class F> filter_map<R, F> operator|(const R& r, filter_mapped<F>&& f) {return {f.fn, r};}
    template <class R, class F> filter_map<R, F> operator|(R&& r, filter_mapped<F>&& f) {return {f.fn, std::move(r)};}
    template <class F, class T = void> struct reduced {
      [[no_unique_address]] F fn;
      T start;
      reduced(F&& fn, T&& start = T()) : fn(std::forward<F>(fn)), start(start) {}
    };
    template <class F> struct reduced<F, void> {
      [[no_unique_address]] F fn;
      reduced(F&& fn) : fn(std::forward<F>(fn)) {}
    };
    template <class R, class F> range_value_t<R> operator|(const R& r, reduced<F, std::remove_cvref_t<decltype(*r.begin())>>&& f) {return std::reduce(r.begin(), r.end(), f.start, f.fn);}
    template <class R, class F> range_value_t<R> operator|(const R& r, reduced<F, void>&& f) {return std::reduce(std::next(r.begin(), 1), r.end(), *r.begin(), f.fn);}
    template <class R, class F> range_value_t<R> reduce(const R& r, F&& f, range_value_t<R>&& start) {return std::reduce(r.begin(), r.end(), start, f);}
    template <class R, class F> range_value_t<R> reduce(const R& r, F&& f) {return std::reduce(std::next(r.begin(), 1), r.end(), *r.begin(), f.fn);}
    template <class F, class T = void> struct accumulated {
      [[no_unique_address]] F fn;
      T start;
      accumulated(F&& fn, T&& start = T()) requires (!std::is_void_v<T>) : fn(std::forward<F>(fn)), start(start) {}
    };
    template <class F> struct accumulated<F, void> {
      [[no_unique_address]] F fn;
      accumulated(F&& fn) : fn(std::forward<F>(fn)) {}
    };
    template <class R, class F> range_value_t<R> operator|(const R& r, accumulated<F, std::remove_cvref_t<decltype(*r.begin())>>&& f) {return std::accumulate(r.begin(), r.end(), f.start, f.fn);}
    template <class R, class F> range_value_t<R> operator|(const R& r, accumulated<F, void>&& f) {return std::accumulate(std::next(r.begin(), 1), r.end(), *r.begin(), f.fn);}
    template <class R, class F> range_value_t<R> accumulate(const R& r, F&& f, range_value_t<R>&& start) {return std::accumulate(r.begin(), r.end(), start, f);}
    template <class R, class F> range_value_t<R> accumulate(const R& r, F&& f) {return std::accumulate(std::next(r.begin(), 1), r.end(), *r.begin(), f.fn);}
    namespace impl {
      template <class T, class = void> struct has_size_t : std::false_type {};
      template <class T> struct has_size_t<T, decltype(std::declval<T>().size())> : std::true_type {};
      template <class T> concept has_size = has_size_t<T>::value;
    }
    template <class T, class F> class sort_it {
      std::deque<T>* const arr;
      mutable bool nodel;
      [[no_unique_address]] F fn;
      void init() {
        auto end = arr->size() - 1;
        {
          auto start = end / 2;
          while (start >= 0) {
            sift(0, end);
            --start;
          }
        }
      }
      void sift(std::size_t root, std::size_t end) {
        while (root * 2 + 1 <= end) {
          auto child = root * 2 + 1;
          auto target = root;
          if (fn(arr[child], arr[target])) target = child;
          if (child + 1 <= end && fn(arr[child + 1], arr[target])) target = child + 1;
          if (target != root) {
            std::swap(arr[root], arr[target]);
            root = target;
          }
          else break;
        }
      }
    public:
      using iterator_category = std::input_iterator_tag;
      using value_type = T;
      using reference = T&;
      using pointer = T*;
      template <class R> sort_it(R&& in, F&& fn) requires std::same_as<range_value_t<R>, T> && impl::has_size<R> : arr(new std::deque<T>(in.size())), fn(fn) {std::move(in.begin(), in.end(), arr->begin()); init();}
      template <class R> sort_it(R&& in, F&& fn) requires std::same_as<range_value_t<R>, T> : arr(new std::deque<T>), fn(fn) {std::move(in.begin(), in.end(), std::back_inserter(*arr)); init();}
      sort_it(const sort_it&) {}
      sort_it(sort_it&&) = default;
      bool operator==(std::default_sentinel_t) const {return arr->size();}
      bool operator!=(std::default_sentinel_t) const {return arr->empty();}
      T& operator*() const {return arr->front();}
      T* operator->() const {return &arr->front();}
      sort_it& operator++() {
        arr->pop_front();
        sift(0, arr->size() - 1);
      }
    };
    template <class F = std::less<void>> struct sorted {
      [[no_unique_address]] F fn;
      sorted(F&& fn) : fn(fn) {}
    };
    template <class T, class F> using sorted_range = boost::iterator_range<std::common_iterator<sort_it<T, F>, std::default_sentinel_t>>;
    template <class R, class F> sorted_range<range_value_t<R>, F> operator|(R&& r, sorted<F>&& f) {return {{r, f}, std::default_sentinel};}
    template <class R, class F> sorted_range<range_value_t<R>, F> lazy_sort(R&& r, F&& f) {return {{r, f}, std::default_sentinel};}
    template <class R> sorted_range<range_value_t<R>, std::less<range_value_t<R>>> lazy_sort(R&& r) {return {{r}, std::default_sentinel};}
  }
}
namespace std {template <class It> struct iterator_traits<mdc::sentinel_iterator<It>> : iterator_traits<It> {};}
#endif // RANGES_HPP
