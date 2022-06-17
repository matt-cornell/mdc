#ifndef MERGE_HPP
#define MERGE_HPP
template <typename T> T merge(T val, T vals...) {
  T v2 = merge<T>(vals);
  T out = val;
  out.insert(out.back(), v2.begin(), v2.end());
  return out;
}
template <typename T> T merge(T val) {
  return val;
}
template <typename T1, template <class, class...> class T2> T1 merge(T2<T1> vals) {
  T2 out;
  for (auto& i : vals) out.insert(out.back(), i.begin(), i.end());
  return out;
}
#endif
