#ifndef PIGEONHOLE_HPP
#define PIGEONHOLE_HPP
#include <cstddef>
#include <iterator>
#include <vector>
namespace mdc {
  constexpr struct vtol_t {template <class T> std::ptrdiff_t operator()(T const& val) const {return static_cast<std::ptrdiff_t>(val);}} vtol;
  template <class I> void ph_sort(I begin, I end) {
    if (begin == end) return;
    std::ptrdiff_t min = PTRDIFF_MAX, max = PTRDIFF_MIN;
    for (auto it = begin; it != end; ++it) {
      auto val = vtol(*it);
      if (val < min) min = val;
      if (val > max) max = val;
    }
    std::vector<std::vector<typename std::iterator_traits<I>::value_type>> vec(max - min + 1);
    for (auto it = begin; it != end; ++it) vec[vtol(*it) - min].push_back(*it);
    auto it = begin;
    for (auto const& v : vec) it = std::copy(v.begin(), v.end(), it);
  }
  template <class I, class F> void ph_sort(I begin, I end, F conv) {
    if (begin == end) return;
    std::ptrdiff_t min = PTRDIFF_MAX, max = PTRDIFF_MIN;
    for (auto it = begin; it != end; ++it) {
      auto val = conv(*it);
      if (val < min) min = val;
      if (val > max) max = val;
    }
    std::vector<std::vector<typename std::iterator_traits<I>::value_type>> vec(max - min + 1);
    for (auto it = begin; it != end; ++it) {
      vec[conv(*it) - min].push_back(*it);
    }
    auto it = begin;
    for (auto const& v : vec) it = std::copy(v.begin(), v.end(), it);
  }
  template <class I, class O> void ph_sort_copy(I begin, I end, O out) {
    if (begin == end) return;
    std::ptrdiff_t min = PTRDIFF_MAX, max = PTRDIFF_MIN;
    for (auto it = begin; it != end; ++it) {
      auto val = vtol(*it);
      if (val < min) min = val;
      if (val > max) max = val;
    }
    std::vector<std::vector<typename std::iterator_traits<I>::value_type>> vec(max - min + 1);
    for (auto it = begin; it != end; ++it) {
      vec[vtol(*it) - min].push_back(*it);
    }
    for (auto const& v : vec) out = std::copy(v.begin(), v.end(), out);
  }
  template <class I, class O, class F> void ph_sort_copy(I begin, I end, O out, F conv) {
    if (begin == end) return;
    std::ptrdiff_t min = PTRDIFF_MAX, max = PTRDIFF_MIN;
    for (auto it = begin; it != end; ++it) {
      auto val = conv(*it);
      if (val < min) min = val;
      if (val > max) max = val;
    }
    std::vector<std::vector<typename std::iterator_traits<I>::value_type>> vec(max - min + 1);
    for (auto it = begin; it != end; ++it) {
      vec[conv(*it) - min].push_back(*it);
    }
    for (auto const& v : vec) out = std::copy(v.begin(), v.end(), out);
  }
  template <class I> void ph_full_sort(I begin, I end) {
    if (begin == end) return;
    std::ptrdiff_t min = PTRDIFF_MAX, max = PTRDIFF_MIN;
    for (auto it = begin; it != end; ++it) {
      auto val = vtol(*it);
      if (val < min) min = val;
      if (val > max) max = val;
    }
    std::vector<std::vector<typename std::iterator_traits<I>::value_type>> vec(max - min + 1);
    for (auto it = begin; it != end; ++it) vec[vtol(*it) - min].push_back(*it);
    auto it = begin;
    for (auto& v : vec) {
      std::sort(v.begin(), v.end());
      it = std::copy(v.begin(), v.end(), it);
    }
  }
  template <class I, class F> void ph_full_sort(I begin, I end, F cmp) {
    if (begin == end) return;
    std::ptrdiff_t min = PTRDIFF_MAX, max = PTRDIFF_MIN;
    for (auto it = begin; it != end; ++it) {
      auto val = vtol(*it);
      if (val < min) min = val;
      if (val > max) max = val;
    }
    std::vector<std::vector<typename std::iterator_traits<I>::value_type>> vec(max - min + 1);
    for (auto it = begin; it != end; ++it) vec[vtol(*it) - min].push_back(*it);
    auto it = begin;
    for (auto& v : vec) {
      std::sort(v.begin(), v.end(), cmp);
      it = std::copy(v.begin(), v.end(), it);
    }
  }
  template <class I, class F1, class F2> void ph_full_sort(I begin, I end, F1 conv, F2 cmp) {
    if (begin == end) return;
    std::ptrdiff_t min = PTRDIFF_MAX, max = PTRDIFF_MIN;
    for (auto it = begin; it != end; ++it) {
      auto val = conv(*it);
      if (val < min) min = val;
      if (val > max) max = val;
    }
    std::vector<std::vector<typename std::iterator_traits<I>::value_type>> vec(max - min + 1);
    for (auto it = begin; it != end; ++it) vec[conv(*it) - min].push_back(*it);
    auto it = begin;
    for (auto& v : vec) {
      std::sort(v.begin(), v.end(), cmp);
      it = std::copy(v.begin(), v.end(), it);
    }
  }
  template <class I, class O> void ph_full_sort_copy(I begin, I end, O out) {
    if (begin == end) return;
    std::ptrdiff_t min = PTRDIFF_MAX, max = PTRDIFF_MIN;
    for (auto it = begin; it != end; ++it) {
      auto val = vtol(*it);
      if (val < min) min = val;
      if (val > max) max = val;
    }
    std::vector<std::vector<typename std::iterator_traits<I>::value_type>> vec(max - min + 1);
    for (auto it = begin; it != end; ++it) vec[vtol(*it) - min].push_back(*it);
    for (auto& v : vec) {
      std::sort(v.begin(), v.end());
      out = std::copy(v.begin(), v.end(), out);
    }
  }
  template <class I, class O, class F> void ph_full_sort_copy(I begin, I end, O out, F cmp) {
    if (begin == end) return;
    std::ptrdiff_t min = PTRDIFF_MAX, max = PTRDIFF_MIN;
    for (auto it = begin; it != end; ++it) {
      auto val = vtol(*it);
      if (val < min) min = val;
      if (val > max) max = val;
    }
    std::vector<std::vector<typename std::iterator_traits<I>::value_type>> vec(max - min + 1);
    for (auto it = begin; it != end; ++it) vec[vtol(*it) - min].push_back(*it);
    for (auto& v : vec) {
      std::sort(v.begin(), v.end(), cmp);
      out = std::copy(v.begin(), v.end(), out);
    }
  }
  template <class I, class O, class F1, class F2> void ph_full_sort_copy(I begin, I end, O out, F1 conv, F2 cmp) {
    if (begin == end) return;
    std::ptrdiff_t min = PTRDIFF_MAX, max = PTRDIFF_MIN;
    for (auto it = begin; it != end; ++it) {
      auto val = conv(*it);
      if (val < min) min = val;
      if (val > max) max = val;
    }
    std::vector<std::vector<typename std::iterator_traits<I>::value_type>> vec(max - min + 1);
    for (auto it = begin; it != end; ++it) vec[conv(*it) - min].push_back(*it);
    for (auto& v : vec) {
      std::sort(v.begin(), v.end(), cmp);
      out = std::copy(v.begin(), v.end(), out);
    }
  }
}
#endif