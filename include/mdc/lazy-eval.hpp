#ifndef LAZY_EVAL_HPP
#define LAZY_EVAL_HPP
#include <memory>
#include <cmath>
#include <tuple>
#include <mdc/misc-utils.hpp>
namespace mdc {
  namespace lazy {
    template <class L, class R> struct assignment;
    template <class L, class R> struct iaddition;
    template <class L, class R> struct isubtraction;
    template <class L, class R> struct imultiplication;
    template <class L, class R> struct idivision;
    template <class L, class R> struct ibit_and;
    template <class L, class R> struct ibit_or;
    template <class L, class R> struct ibit_xor;
    template <class L, class R> struct ileft_shift;
    template <class L, class R> struct iright_shift;
    template <class T> struct value {
      T operator()() const {return eval();}
      virtual T eval() const = 0;
      virtual std::shared_ptr<value<T>> to_heap() && = 0;
      virtual std::shared_ptr<value<T>> to_heap() const& = 0;
#define DEF_IOP(NAME, OP) \
      template <class T2> NAME<T, T2> operator OP ## =(const value<T2>& other) const;\
      template <class T2> NAME<T, T2> operator OP ## =(value<T2>&& other) const;
      DEF_IOP(assignment,)
      DEF_IOP(iaddition, +)
      DEF_IOP(isubtraction, -)
      DEF_IOP(imultiplication, *)
      DEF_IOP(idivision, /)
      DEF_IOP(ibit_and, &)
      DEF_IOP(ibit_or, |)
      DEF_IOP(ibit_xor, ^)
      DEF_IOP(ileft_shift, <<)
      DEF_IOP(iright_shift, >>)
#undef DEF_IOP
    };
    template <class T> struct constant : value<T> {
      T val;
      constant(const T& val_arg) : val(val_arg) {}
      constant(T&& val_arg) : val(std::move(val_arg)) {}
      constant(const constant<T>&) = default;
      virtual T eval() const {return val;}
      virtual std::shared_ptr<value<T>> to_heap() && {return std::make_shared<constant<T>>(std::move(*this));}
      virtual std::shared_ptr<value<T>> to_heap() const& {return std::make_shared<constant<T>>(*this);}
    };
    template <class T> struct variable : value<T&> {
      T& val;
      variable(T& arg) : val(arg) {}
      virtual T& eval() const {return val;}
      virtual std::shared_ptr<value<T>> to_heap() && {return std::make_shared<variable<T>>(std::move(*this));}
      virtual std::shared_ptr<value<T>> to_heap() const& {return std::make_shared<variable<T>>(*this);}
    };
    namespace op_types {
#define DEF_OP(NAME, OP) template <class L, class R> using NAME = decltype(std::declval<L>() OP std::declval<R>());
      DEF_OP(addition_of, +)
      DEF_OP(subtraction_of, -)
      DEF_OP(multiplication_of, *)
      DEF_OP(division_of, /)
      DEF_OP(bit_and_of, &)
      DEF_OP(bit_or_of, |)
      DEF_OP(bit_xor_of, ^)
      DEF_OP(left_shift_of, <<)
      DEF_OP(right_shift_of, >>)
      DEF_OP(less_than_of, <)
      DEF_OP(greater_than_of, >)
      DEF_OP(less_equal_of, <=)
      DEF_OP(greater_equal_of, >=)
      DEF_OP(equal_to_of, ==)
      DEF_OP(not_equal_of, !=)
      DEF_OP(logic_and_of, &&)
      DEF_OP(logic_or_of, ||)
      DEF_OP(iaddition_of, +=)
      DEF_OP(isubtraction_of, -=)
      DEF_OP(imultiplication_of, *=)
      DEF_OP(idivision_of, /=)
      DEF_OP(ibit_and_of, &=)
      DEF_OP(ibit_or_of, |=)
      DEF_OP(ibit_xor_of, ^=)
      DEF_OP(ileft_shift_of, <<=)
      DEF_OP(iright_shift_of, >>=)
#undef DEF_OP
      template <class L, class R> using modulus_of = decltype(mdc::modulus(std::declval<L>(), std::declval<R>()));
      template <class T> using identity_of = decltype(+std::declval<T>());
      template <class T> using negation_of = decltype(-std::declval<T>());
      template <class T> using conjugate_of = decltype(~std::declval<T>());
      template <class T> using logic_neg_of = decltype(!std::declval<T>());
      template <class T> using pre_incr_of = decltype(++std::declval<T>());
      template <class T> using post_incr_of = decltype(std::declval<T>()++);
      template <class T> using pre_decr_of = decltype(--std::declval<T>());
      template <class T> using post_decr_of = decltype(std::declval<T>()--);
      template <class T> using indirection_of = decltype(*std::declval<T>());
    }
#define DEF_CLASS(NAME, OP) \
    template <class L, class R> struct NAME : value<op_types::NAME ## _of<L, R>> {\
      std::shared_ptr<const value<L>> lhs;\
      std::shared_ptr<const value<R>> rhs;\
      NAME(std::shared_ptr<const value<L>>&& l_arg, std::shared_ptr<const value<R>>&& r_arg) : lhs(std::move(l_arg)), rhs(std::move(r_arg)) {}\
      virtual op_types::NAME ## _of<L, R> eval() const {return lhs->eval() OP rhs->eval();}\
      virtual std::shared_ptr<value<op_types::NAME ## _of<L, R>>> to_heap() && {return std::make_shared<NAME<L, R>>(std::move(*this));}\
      virtual std::shared_ptr<value<op_types::NAME ## _of<L, R>>> to_heap() const& {return std::make_shared<NAME<L, R>>(*this);}\
    };
    DEF_CLASS(addition, +)
    DEF_CLASS(subtraction, -)
    DEF_CLASS(multiplication, *)
    DEF_CLASS(division, /)
    DEF_CLASS(bit_and, &)
    DEF_CLASS(bit_or, |)
    DEF_CLASS(bit_xor, ^)
    DEF_CLASS(left_shift, <<)
    DEF_CLASS(right_shift, >>)
    DEF_CLASS(less_than, <)
    DEF_CLASS(greater_than, >)
    DEF_CLASS(less_equal, <=)
    DEF_CLASS(greater_equal, >=)
    DEF_CLASS(equal_to, ==)
    DEF_CLASS(not_equal, !=)
    DEF_CLASS(logic_and, &&)
    DEF_CLASS(logic_or, ||)
    DEF_CLASS(iaddition, +=)
    DEF_CLASS(isubtraction, -=)
    DEF_CLASS(imultiplication, *=)
    DEF_CLASS(idivision, /=)
    DEF_CLASS(ibit_and, &=)
    DEF_CLASS(ibit_or, |=)
    DEF_CLASS(ibit_xor, ^=)
    DEF_CLASS(ileft_shift, <<=)
    DEF_CLASS(iright_shift, >>=)
#undef DEF_CLASS
    template <class L, class R> struct modulus : value<op_types::modulus_of<L, R>> {
      std::shared_ptr<const value<L>> lhs;
      std::shared_ptr<const value<R>> rhs;
      modulus(std::shared_ptr<const value<L>>&& l_arg, std::shared_ptr<const value<R>>&& r_arg) : lhs(std::move(l_arg)), rhs(std::move(r_arg)) {}
      virtual op_types::modulus_of<L, R> eval() const {return modulus(lhs->eval(), rhs->eval());}
      virtual std::shared_ptr<value<op_types::modulus_of<L, R>>> to_heap() && {return std::make_shared<modulus<L, R>>(std::move(*this));}
      virtual std::shared_ptr<value<op_types::modulus_of<L, R>>> to_heap() const& {return std::make_shared<modulus<L, R>>(*this);}
    };
#define DEF_CLASS(NAME, OP) \
    template <class T> struct NAME : value<op_types::NAME ## _of<T>> {\
      std::shared_ptr<const value<T>> val;\
      NAME(std::shared_ptr<const value<T>>&& val_arg) : val(std::move(val_arg)) {}\
      virtual op_types::NAME ## _of<T> eval() const {return OP val->eval();}\
      virtual std::shared_ptr<value<T>> to_heap() && {return std::make_shared<NAME<T>>(std::move(*this));}\
      virtual std::shared_ptr<value<T>> to_heap() const& {return std::make_shared<NAME<T>>(*this);}\
    };
    DEF_CLASS(identity, +)
    DEF_CLASS(negation, -)
    DEF_CLASS(conjugate, ~)
    DEF_CLASS(logic_neg, !)
    DEF_CLASS(pre_incr, ++)
    DEF_CLASS(pre_decr, --)
    DEF_CLASS(indirection, *)
#undef DEF_CLASS
    template <class T> struct post_incr : value<op_types::post_incr_of<T>> {
      std::shared_ptr<const value<T>> val;
      post_incr(std::shared_ptr<const value<T>>&& val_arg) : val(std::move(val_arg)) {}
      virtual op_types::post_incr_of<T> eval() const {return val->eval()++;}
      virtual std::shared_ptr<value<T>> to_heap() && {return std::make_shared<post_incr<T>>(std::move(*this));}
      virtual std::shared_ptr<value<T>> to_heap() const& {return std::make_shared<post_incr<T>>(*this);}
    };
    template <class T> struct post_decr : value<op_types::post_decr_of<T>> {
      std::shared_ptr<const value<T>> val;
      post_decr(std::shared_ptr<const value<T>>&& val_arg) : val(std::move(val_arg)) {}
      virtual op_types::post_decr_of<T> eval() const {return val->eval()--;}
      virtual std::shared_ptr<value<T>> to_heap() && {return std::make_shared<post_decr<T>>(std::move(*this));}
      virtual std::shared_ptr<value<T>> to_heap() const& {return std::make_shared<post_decr<T>>(*this);}
    };
    namespace impl {
      template <template <class> class, class> struct type_transform;
      template <template <class> class F, class... Args> struct type_transform<F, std::tuple<Args...>> {using type = std::tuple<typename F<Args>::type...>;};
      template <class F, class T, std::size_t... Is> auto transform_each_impl(T&& t, F&& f, std::index_sequence<Is...>) {return std::make_tuple(f(std::get<Is>(t))...);}
      template <class F, class... As> auto tuple_transform(const std::tuple<As...>& t, F&& f = F()) {return transform_each_impl(t, f, std::make_index_sequence<sizeof...(As)>{});}
    }
    template <class F, class... Args> struct lazy_call : value<std::invoke_result_t<F, Args...>> {
      std::shared_ptr<const value<F>> fn;
      std::tuple<std::shared_ptr<const value<Args>>...> args;
      lazy_call(std::shared_ptr<const value<F>>&& fn_arg, std::tuple<std::shared_ptr<const value<Args>>...>&& arg_args) : fn(std::move(fn_arg)), args(std::move(arg_args)) {}
      lazy_call(std::shared_ptr<const value<F>>&& fn_arg, std::shared_ptr<const value<Args>>&&... arg_args) : fn(std::move(fn_arg)), args(std::make_tuple(arg_args...)) {}
      virtual std::invoke_result_t<F, Args...> eval() const {return std::apply(fn->eval(), impl::tuple_transform(args, [](const auto& in) {return in->eval();}));}
      virtual std::shared_ptr<value<std::invoke_result_t<F, Args...>>> to_heap() && {return std::make_shared<lazy_call<F, Args...>>(std::move(*this));}
      virtual std::shared_ptr<value<std::invoke_result_t<F, Args...>>> to_heap() const& {return std::make_shared<lazy_call<F, Args...>>(*this);}
    };
    template <class T, class F> struct cast : value<T> {
      std::shared_ptr<const value<F>> from;
      cast(std::shared_ptr<const value<F>>&& from_arg) : from(std::move(from_arg)) {}
      virtual T eval() const {return static_cast<T>(from->eval());}
      virtual std::shared_ptr<value<T>> to_heap() && {return std::make_shared<cast<T, F>>(std::move(*this));}
      virtual std::shared_ptr<value<T>> to_heap() const& {return std::make_shared<lazy_call<T, F>>(*this);}
    };
    template <class T> struct wrap_ptr : value<T> {
      std::shared_ptr<const value<T>> ptr;
      wrap_ptr(std::shared_ptr<const value<T>>&& ptr_arg) : ptr(std::move(ptr_arg)) {}
      virtual T eval() const {return ptr->eval();}
    };
    namespace impl {
      template <class, class... Args> struct get_last {using type = typename get_last<Args...>::type;};
      template <class T> struct get_last<T> {using type = T;};
      template <class... Args> using get_last_t = typename get_last<Args...>::type;
    }
    template <class... Types> struct compound : value<impl::get_last_t<Types...>> {
      std::tuple<std::shared_ptr<const value<Types>>...> instructions;
      compound(std::tuple<std::shared_ptr<const value<Types>>...>&& inst_args) : instructions(std::move(inst_args)) {}
      compound(std::shared_ptr<const value<Types>>&&... inst_args) : instructions(std::move(inst_args)...) {}
      compound(std::tuple<value<Types>...>&& inst_args) : instructions(std::move(inst_args)) {}
      compound(value<Types>&&... inst_args) : instructions(std::move(inst_args).to_heap()...) {}
      virtual impl::get_last_t<Types...> eval() const {return std::get<sizeof...(Types) - 1>(impl::tuple_transform(instructions, [](const auto& in) {return in.eval();}));}
      virtual std::shared_ptr<compound<Types...>> to_heap() && {return std::make_shared<compound<Types...>>(std::move(*this));}
      virtual std::shared_ptr<compound<Types...>> to_heap() const& {return std::make_shared<compound<Types...>>(*this);}
    };
    template <class T, class O> struct ternary : value<O> {
      std::shared_ptr<const value<T>> cond;
      std::shared_ptr<const value<O>> if_true, if_false;
      virtual O eval() const {return cond->eval() ? if_true->eval() : if_false->eval();}
    };
    template <class F> struct lazy_fn : constant<F> {
      lazy_fn(F&& fn) : constant<F>(std::move(fn)) {}
      template <class... Args> lazy_call<F, Args...> operator()(value<Args>&&... args) const {return {to_heap(), std::move(args)...};}
      template <class... Args> lazy_call<F, Args...> operator()(const value<Args>&... args) const {return {to_heap(), args...};}
      virtual std::shared_ptr<lazy_fn<F>> to_heap() && {return std::make_shared<compound<F>>(std::move(*this));}
      virtual std::shared_ptr<lazy_fn<F>> to_heap() const& {return std::make_shared<compound<F>>(*this);}
    };
#define DEF_IOP(NAME, OP) \
  template <class T> template <class T2> NAME<T, T2> value<T>::operator OP ## =(const value<T2>& other) const {return {std::shared_ptr<const value<T>>(to_heap()), std::shared_ptr<const value<T2>>(other.to_heap())};}\
  template <class T> template <class T2> NAME<T, T2> value<T>::operator OP ## =(value<T2>&& other) const {return {std::shared_ptr<const value<T>>(to_heap()), std::shared_ptr<const value<T2>>(std::move(other).to_heap())};}
    DEF_IOP(assignment,)
    DEF_IOP(iaddition, +)
    DEF_IOP(isubtraction, -)
    DEF_IOP(imultiplication, *)
    DEF_IOP(idivision, /)
    DEF_IOP(ibit_and, &)
    DEF_IOP(ibit_or, |)
    DEF_IOP(ibit_xor, ^)
    DEF_IOP(ileft_shift, <<)
    DEF_IOP(iright_shift, >>)
    inline namespace operators {
#define DEF_OP(NAME, OP) \
      template <class L, class R> NAME<L, R> operator OP(value<L>&& lhs, value<R>&& rhs) {return {std::shared_ptr<const value<L>>(lhs.to_heap()), std::shared_ptr<const value<R>>(rhs.to_heap())};}\
      template <class L, class R> NAME<L, R> operator OP(const value<L>& lhs, value<R>&& rhs) {return {std::shared_ptr<const value<L>>(lhs.to_heap()), std::shared_ptr<const value<R>>(rhs.to_heap())};}\
      template <class L, class R> NAME<L, R> operator OP(value<L>&& lhs, const value<R>& rhs) {return {std::shared_ptr<const value<L>>(lhs.to_heap()), std::shared_ptr<const value<R>>(rhs.to_heap())};}\
      template <class L, class R> NAME<L, R> operator OP(const value<L>& lhs, const value<R>& rhs) {return {std::shared_ptr<const value<L>>(lhs.to_heap()), std::shared_ptr<const value<R>>(rhs.to_heap())};}
      DEF_OP(addition, +)
      DEF_OP(subtraction, -)
      DEF_OP(multiplication, *)
      DEF_OP(division, /)
      DEF_OP(modulus, %)
      DEF_OP(bit_and, &)
      DEF_OP(bit_or, |)
      DEF_OP(bit_xor, ^)
      DEF_OP(left_shift, <<)
      DEF_OP(right_shift, >>)
      DEF_OP(less_than, <)
      DEF_OP(greater_than, >)
      DEF_OP(less_equal, <=)
      DEF_OP(greater_equal, >=)
      DEF_OP(equal_to, ==)
      DEF_OP(not_equal, !=)
#undef DEF_OP
#define DEF_OP(NAME, OP) \
      template <class T> NAME<T> operator OP(value<T>&& val) {return {std::shared_ptr<const value<T>>(val.to_heap())};}\
      template <class T> NAME<T> operator OP(const value<T>& val) {return {std::shared_ptr<const value<T>>(val.to_heap())};}
      DEF_OP(identity, +)
      DEF_OP(negation, -)
      DEF_OP(conjugate, ~)
      DEF_OP(logic_neg, !)
      DEF_OP(pre_incr, ++)
      DEF_OP(pre_decr, --)
      DEF_OP(indirection, *)
#undef DEF_OP
      template <class T> post_incr<T> operator++(value<T>&& val, int) {return {std::shared_ptr<const value<T>>(val.to_heap())};}
      template <class T> post_incr<T> operator++(const value<T>& val, int) {return {std::shared_ptr<const value<T>>(val.to_heap())};}
      template <class T> post_decr<T> operator--(value<T>&& val, int) {return {std::shared_ptr<const value<T>>(val.to_heap())};}
      template <class T> post_decr<T> operator--(const value<T>& val, int) {return {std::shared_ptr<const value<T>>(val.to_heap())};}
      template <class T, class F> cast<T, F> lazy_cast(value<T>&& val) {return {std::shared_ptr<const value<T>>(val.to_heap())};}
      template <class T, class F> cast<T, F> lazy_cast(const value<T>& val) {return {std::shared_ptr<const value<T>>(val.to_heap())};}
    }
    auto $(auto&& arg)->constant<std::remove_reference_t<decltype(arg)>> {return std::move(arg);}
    auto $(auto& arg)->variable<std::remove_reference_t<decltype(arg)>> {return arg;}
    auto c$(auto arg)->constant<std::remove_cvref_t<decltype(arg)>> {return std::move(arg);}
  }
  inline namespace literals {
    inline namespace lazy_literals {
      lazy::constant<long long> operator""_l(unsigned long long in) {return lazy::constant<long long>{std::move(in)};}
      lazy::constant<long double> operator""_l(long double in) {return lazy::constant<long double>(std::move(in));}
    }
  }
}
#endif // LAZY_EVAL_HPP
