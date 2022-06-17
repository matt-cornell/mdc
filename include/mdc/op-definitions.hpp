#ifndef OP_DEFINITIONS_HPP
#define OP_DEFINITIONS_HPP
namespace operations {
  template <class T> struct addition {};
  template <class T> struct subtraction {};
  template <class T> struct multiplication {};
  template <class T> struct division {};
  template <class T> struct modulus {};
  template <class T> struct less_than {};
  template <class T> struct greater_than {};
  template <class T> struct less_equal {};
  template <class T> struct greater_equal {};
  template <class T> struct equal_to {};
  template <class T> struct not_equal {};
  template <class T> struct bit_and {};
  template <class T> struct bit_or {};
  template <class T> struct left_shift {};
  template <class T> struct right_shift {};
  template <class T> struct r_addition {};
  template <class T> struct r_subtraction {};
  template <class T> struct r_multiplication {};
  template <class T> struct r_division {};
  template <class T> struct r_less_than {};
  template <class T> struct r_greater_than {};
  template <class T> struct r_less_equal {};
  template <class T> struct r_greater_equal {};
  template <class T> struct r_equal_to {};
  template <class T> struct r_not_equal {};
  template <class T> struct r_bit_and {};
  template <class T> struct r_bit_or {};
  template <class T> struct r_modulus {};
  template <class T> struct r_left_shift {};
  template <class T> struct r_right_shift {};
};
constexpr struct self_t {} self;
template <class T> struct type_wrapper {};
template <class T> constexpr type_wrapper<T> val_of;
template <class T> constexpr operations::addition<T> operator+(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::subtraction<T> operator-(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::multiplication<T> operator*(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::division<T> operator/(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::modulus<T> operator%(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::less_than<T> operator<(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::greater_than<T> operator>(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::less_equal<T> operator<=(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::greater_equal<T> operator>=(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::equal_to<T> operator==(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::not_equal<T> operator!=(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::bit_and<T> operator&(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::bit_or<T> operator|(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::left_shift<T> operator<<(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::right_shift<T> operator>>(self_t, type_wrapper<T>) {return {};}
template <class T> constexpr operations::r_addition<T> operator+(type_wrapper<T>, self_t) {return {};}
template <class T> constexpr operations::r_subtraction<T> operator-(type_wrapper<T>, self_t) {return {};}
template <class T> constexpr operations::r_multiplication<T> operator*(type_wrapper<T>, self_t) {return {};}
template <class T> constexpr operations::r_division<T> operator/(type_wrapper<T>, self_t) {return {};}
template <class T> constexpr operations::r_modulus<T> operator%(type_wrapper<T>, self_t) {return {};}
template <class T> constexpr operations::r_less_than<T> operator<(type_wrapper<T>, self_t) {return {};}
template <class T> constexpr operations::r_greater_than<T> operator>(type_wrapper<T>, self_t) {return {};}
template <class T> constexpr operations::r_less_equal<T> operator<=(type_wrapper<T>, self_t) {return {};}
template <class T> constexpr operations::r_greater_equal<T> operator>=(type_wrapper<T>, self_t) {return {};}
template <class T> constexpr operations::r_equal_to<T> operator==(type_wrapper<T>, self_t) {return {};}
template <class T> constexpr operations::r_not_equal<T> operator!=(type_wrapper<T>, self_t) {return {};}
template <class T> constexpr operations::r_bit_and<T> operator&(type_wrapper<T>, self_t) {return {};}
template <class T> constexpr operations::r_bit_or<T> operator|(type_wrapper<T>, self_t) {return {};}
template <class T> constexpr operations::r_left_shift<T> operator<<(type_wrapper<T>, self_t) {return {};}
template <class T> constexpr operations::r_right_shift<T> operator>>(type_wrapper<T>, self_t) {return {};}
#endif // OP_DEFINITIONS_HPP
