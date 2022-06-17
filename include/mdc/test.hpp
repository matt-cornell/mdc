#ifndef TEST_HPP
#define TEST_HPP
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <variant>
#include <vector>
#include <mdc/borrow-func.hpp>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/variadic/to_list.hpp>
namespace mdc {
  namespace test {
    namespace randgen {
  #ifndef MDC_CONTAINER_MAX_SIZE
      std::size_t max_container_size = 1000;
  #else
      std::size_t max_container_size = MDC_CONTAINER_MAX_SIZE;
  #endif
      template <class T> struct static_functor {
        inline static T fn;
        auto operator()() {return fn();}
      };
      template <class T, class R = static_functor<std::random_device>> struct generator;
      template <class T, class R> struct generator<T const, R> : generator<T, R> {};
      template <class T, class R> struct generator<T&, R> : generator<T, R> {};
      template <class T, class R> struct generator<T const&, R> : generator<T, R> {};
      template <class T, class R = std::random_device> struct gen_closure {
        [[no_unique_address]] generator<T, R> gen;
        R rand;
        gen_closure(R rand) : rand(rand) {}
        T operator()() {return gen(rand);}
        void operator()(T& val) {gen(val, rand);}
      };
  #define __MDC_DEF_RANDGEN(T) \
      template <class R> struct generator<T, R> { \
        constexpr static bool has_new = true, has_inplace = true; \
        T operator()(R& rand = R{}) {auto val = rand(); return reinterpret_cast<T&>(val);} \
        void operator()(T& val, R& rand = R{}) {val = (*this)(rand);} \
      };
      __MDC_DEF_RANDGEN(int8_t)
      __MDC_DEF_RANDGEN(int16_t)
      __MDC_DEF_RANDGEN(int32_t)
      __MDC_DEF_RANDGEN(int64_t)
      __MDC_DEF_RANDGEN(uint8_t)
      __MDC_DEF_RANDGEN(uint16_t)
      __MDC_DEF_RANDGEN(uint32_t)
      __MDC_DEF_RANDGEN(uint64_t)
      __MDC_DEF_RANDGEN(char8_t)
      __MDC_DEF_RANDGEN(char16_t)
      __MDC_DEF_RANDGEN(char32_t)
      __MDC_DEF_RANDGEN(float)
      __MDC_DEF_RANDGEN(double)
  #undef __MDC_DEF_RANDGEN
      template <class T, std::size_t sz, class R> struct generator<T[sz], R> {
        constexpr static bool has_new = false, has_inplace = true;
        void operator()(T val[sz], R& rand) {
          generator<T, R> r2{};
          static_assert (generator<T, R>::has_inplace || generator<T, R>::has_new, "generator must be able to return a value or generate one in place");
          for (T* it = val; it != val + sz; ++it) if constexpr(generator<T, R>::has_inplace) r2(*it, rand);
          else if constexpr (generator<T, R>::has_new) *it = r2(rand);
        }
      };
      template <class R, class... As> struct generator<std::tuple<As...>, R> {
        constexpr static bool has_new = true, has_inplace = true;
        std::tuple<As...> operator()(R rand) {
          std::tuple<As...> val;
          gen_elems(val, rand, std::make_index_sequence<sizeof...(As)>{});
          return val;
        }
        void operator()(std::tuple<As...> val, R rand) {gen_elems(val, rand, std::make_index_sequence<sizeof...(As)>{});}
      private:
        template <std::size_t idx> static char gen_elem(std::tuple<As...>& tup, R rand) {generator<std::tuple_element_t<idx, std::tuple<As...>>, R>{}(std::get<idx>(tup), rand);}
        template <std::size_t... indices> static void gen_elems(std::tuple<As...>& tup, R rand, std::index_sequence<indices...>) {nullfunc(gen_elem<indices>(tup, rand)...);}
        static void nullfunc(...) {}
      };
      template <class T> struct pod_blacklist : std::false_type {};
      template <class T, class R> requires (std::is_pod_v<T> && !pod_blacklist<T>::value) struct generator<T, R> {
        constexpr static bool has_new = true, has_inplace = true;
        T operator()(R rand) {
          T val;
          generator<int8_t[sizeof(T)], R> gen;
          gen(reinterpret_cast<int8_t[sizeof(T)]>(val), rand);
          return val;
        }
        void operator()(T& val, R rand) {
          using arr = int8_t[sizeof(T)];
          generator<arr&, R> gen;
          gen(reinterpret_cast<arr&>(val), rand);
        }
      };
      template <class T, class R> struct generator<std::vector<T>, R> {
        std::vector<T> operator()() {
          static_assert (generator<T, R>::has_inplace || generator<T, R>::has_new, "generator must be able to return a value or generate one in place");
          std::size_t size;
          {
            generator<std::size_t, R> gen{};
            gen(size, rand);
            if (max_container_size) while (size < max_container_size) gen(size, rand);
          }
          std::vector<T> out(size);
          generator<T, R> gen{rand};
          if constexpr (generator<T, R>::has_inplace) std::for_each(out.begin(), out.end(), gen);
          else std::generate(out.begin(), out.end(), gen);
          return out;
        }
      };
      template <class T, class R> struct generator<std::basic_string<T>, R> {
        R rand;
        template <class... As> generator(As&&... args) : rand(args...) {}
        std::basic_string<T> operator()() {
          static_assert (generator<T, R>::has_inplace || generator<T, R>::has_new, "generator must be able to return a value or generate one in place");
          std::size_t size;
          {
            generator<std::size_t, R> gen{rand};
            gen(size);
            if (max_container_size) while (size < max_container_size) gen(size);
          }
          std::vector<T> out(size);
          generator<T, R> gen{rand};
          if constexpr (generator<T, R>::has_inplace) std::for_each(out.begin(), out.end(), gen);
          else std::generate(out.begin(), out.end(), gen);
          return out;
        }
      };
      namespace detail {
        template <class T> struct get_type;
        template <class T, class R> struct get_type<void(T, R)> {using type = T;};
        template <class T> using get_type_t = typename get_type<T>::type;
        template <class T> struct get_rand;
        template <class T, class R> struct get_rand<void(T, R)> {using type = R;};
        template <class T> using get_rand_t = typename get_rand<T>::type;
      }
    }
    inline namespace test_builders {struct finish_t;}
    struct test_case : std::function<bool()> {
      test_case() = delete;
    private:
      template <class F> test_case(F&& fn) : std::function<bool()>(std::move(fn)) {}
      friend struct test_builders::finish_t;
    };
    constexpr struct default_predicate_t {
      template <class T> bool operator()(T&& arg) const {return static_cast<bool>(std::move(arg));}
      template <class T> bool operator()(T const& arg) const {return static_cast<bool>(arg);}
    } default_predicate;

    inline namespace test_builders {
      struct default_test_config {
        using exception = std::nothrow_t;
        using args_type = std::tuple<>;
        using pred_type = default_predicate_t;
        constexpr static bool invert = false;
      };
      namespace {
        template <class C, class args_t> struct set_args {
          using args_type = args_t;
          using exception = typename C::exception;
          using pred_type = typename C::pred_type;
          constexpr static bool invert = C::invert;
        };
        template <class C, class exception_> struct set_ex {
          using args_type = typename C::args_type;
          using exception = exception_;
          using pred_type = typename C::pred_type;
          constexpr static bool invert = C::invert;
        };
        template <class C> struct set_invert {
          using args_type = typename C::args_type;
          using exception = typename C::exception;
          using pred_type = typename C::pred_type;
          constexpr static bool invert = true;
        };
        template <class C, class P> struct set_predicate {
          using args_type = typename C::args_type;
          using exception = typename C::exception;
          using pred_type = P;
          constexpr static bool invert = C::invert;
        };
        template <class F, class A> struct apply_result;
        template <class F, class... As> struct apply_result<F, std::tuple<As...>> {using type = std::invoke_result_t<F, As...>;};
        template <class F, class A> using apply_result_t = typename apply_result<F, A>::type;
      }
      template <class T> concept is_test_config = requires {
        {std::declval<typename T::exception>()};
        {std::declval<typename T::args_type>()};
        {std::declval<typename T::eq_functior>()};
        {T::invert} -> std::same_as<bool>;
      };
      template <class T> concept is_test = is_test_config<typename T::test_type>;
      template <class F, class C> struct test_data {
        using func_type = F;
        using test_type = C;
        F fn;
        std::variant<typename C::args_type, std::size_t> args = typename C::args_type {};
        typename C::pred_type pred = {};
      };
      template <class C> struct test_args {
        std::variant<typename C::args_type, std::size_t> args = typename C::args_type {};
        template <class F> test_data<F, C> operator()(F&& fn) & {return {std::move(fn), args};}
        template <class F> test_data<F, C> operator()(F const& fn) & {return {fn, args};}
        template <class F> test_data<F, C> operator()(F&& fn) && {return {std::move(fn), std::move(args)};}
        template <class F> test_data<F, C> operator()(F const& fn) && {return {fn, std::move(args)};}
      };
      constexpr struct test_t {
        template <class F> test_data<F, default_test_config> operator()(F&& fn) const {return {std::move(fn), {}};}
        template <class F> test_data<F, default_test_config> operator()(F const& fn) const {return {fn, {}};}
        inline operator test_args<default_test_config>() const {return {};}
      } test;
      template <class F> inline test_data<F, default_test_config> operator|(F&& fn, test_t) {return {std::move(fn), {}};}
      template <class F> inline test_data<F, default_test_config> operator-(F&& fn, test_t) {return {std::move(fn), {}};}
      template <class F> inline test_data<F, default_test_config> operator|(F const& fn, test_t) {return {fn, {}};}
      template <class F> inline test_data<F, default_test_config> operator-(F const& fn, test_t) {return {fn, {}};}
      constexpr struct args_t {
        template <class... args_t> struct closure {std::tuple<args_t...> args;};
        template <class... args_t> closure<args_t...> operator()(args_t&&... args) const {return {std::make_tuple(args...)};}
        template <class F, class C, class... args_t> set_args<F, std::tuple<args_t...>> operator()(test_data<F, C>&& test, args_t&&... args) const {return {std::move(test.fn), std::make_tuple(std::move(args)...), std::move(test.pred)};}
        template <class F, class C, class... args_t> set_args<F, std::tuple<args_t...>> operator()(test_data<F, C> const& test, args_t&&... args) const {return {test.fn, std::make_tuple(std::move(args)...), test.pred};}
      } args;
      template <class... As> struct typed_args_t {
        typename args_t::template closure<As...> operator()(As&&... args) const {return {std::make_tuple(args...)};}
        template <class F, class C> set_args<F, std::tuple<As...>> operator()(test_data<F, C>&& test, As&&... args) const {return {std::move(test.fn), std::make_tuple(std::move(args)...), std::move(test.pred)};}
        template <class F, class C> set_args<F, std::tuple<As...>> operator()(test_data<F, C> const& test, As&&... args) const {return {test.fn, std::make_tuple(std::move(args)...), test.pred};}
      };
      template <class... args_t> constexpr typed_args_t<args_t...> typed_args;
      template <class F, class C, class... As> inline test_data<F, set_args<C, std::tuple<As...>>> operator|(test_data<F, C>&& test, args_t::closure<As...> const& args) {return {std::move(test.fn), args.args, std::move(test.pred)};}
      template <class F, class C, class... As> inline test_data<F, set_args<C, std::tuple<As...>>> operator-(test_data<F, C>&& test, args_t::closure<As...> const& args) {return {std::move(test.fn), args.args, std::move(test.pred)};}
      template <class F, class C, class... As> inline test_data<F, set_args<C, std::tuple<As...>>> operator|(test_data<F, C> const& test, args_t::closure<As...> const& args) {return {test.fn, args.args, test.pred};}
      template <class F, class C, class... As> inline test_data<F, set_args<C, std::tuple<As...>>> operator-(test_data<F, C> const& test, args_t::closure<As...> const& args) {return {test.fn, args.args, test.pred};}
      template <class F, class C, class... As> inline test_data<F, set_args<C, std::tuple<As...>>> operator|(test_data<F, C>&& test, args_t::closure<As...>&& args) {return {std::move(test.fn), std::move(args.args), std::move(test.pred)};}
      template <class F, class C, class... As> inline test_data<F, set_args<C, std::tuple<As...>>> operator-(test_data<F, C>&& test, args_t::closure<As...>&& args) {return {std::move(test.fn), std::move(args.args), std::move(test.pred)};}
      template <class F, class C, class... As> inline test_data<F, set_args<C, std::tuple<As...>>> operator|(test_data<F, C> const& test, args_t::closure<As...>&& args) {return {test.fn, std::move(args.args), test.pred};}
      template <class F, class C, class... As> inline test_data<F, set_args<C, std::tuple<As...>>> operator-(test_data<F, C> const& test, args_t::closure<As...>&& args) {return {test.fn, std::move(args.args), test.pred};}
      template <class C, class... As> inline test_args<set_args<C, std::tuple<As...>>> operator|(test_args<C>&&, args_t::closure<As...> const& args) {return {args.args};}
      template <class C, class... As> inline test_args<set_args<C, std::tuple<As...>>> operator-(test_args<C>&&, args_t::closure<As...> const& args) {return {args.args};}
      template <class C, class... As> inline test_args<set_args<C, std::tuple<As...>>> operator|(test_args<C> const&, args_t::closure<As...> const& args) {return {args.args};}
      template <class C, class... As> inline test_args<set_args<C, std::tuple<As...>>> operator-(test_args<C> const&, args_t::closure<As...> const& args) {return {args.args};}
      template <class C, class... As> inline test_args<set_args<C, std::tuple<As...>>> operator|(test_args<C>&&, args_t::closure<As...>&& args) {return {std::move(args.args)};}
      template <class C, class... As> inline test_args<set_args<C, std::tuple<As...>>> operator-(test_args<C>&&, args_t::closure<As...>&& args) {return {std::move(args.args)};}
      template <class C, class... As> inline test_args<set_args<C, std::tuple<As...>>> operator|(test_args<C> const&, args_t::closure<As...>&& args) {return {std::move(args.args)};}
      template <class C, class... As> inline test_args<set_args<C, std::tuple<As...>>> operator-(test_args<C> const&, args_t::closure<As...>&& args) {return {std::move(args.args)};}
      constexpr struct fuzz_t {
        struct closure {std::size_t val;};
        closure operator()(std::size_t val) const {return {val};}
        template <is_test T> T operator()(T&& test, std::size_t fuzz_count) const {test.args = fuzz_count; return test;}
        template <is_test T> T operator()(T const& test, std::size_t fuzz_count) const {T test_ = test; test_.args = fuzz_count; return test_;}
      } fuzz;
      template <class F, class C> inline test_data<F, C> operator|(test_data<F, C>&& test, fuzz_t::closure cls) {test.args = cls.val; return test;}
      template <class F, class C> inline test_data<F, C> operator-(test_data<F, C>&& test, fuzz_t::closure cls) {test.args = cls.val; return test;}
      template <class F, class C> inline test_data<F, C> operator|(test_data<F, C> const& test, fuzz_t::closure cls) {test_data<F, C> test_ = test; test_.args = cls.val; return test_;}
      template <class F, class C> inline test_data<F, C> operator-(test_data<F, C> const& test, fuzz_t::closure cls) {test_data<F, C> test_ = test; test_.args = cls.val; return test_;}
      template <class F, class C> inline test_args<C> operator|(test_args<C>&& test, fuzz_t::closure cls) {test.args = cls.val; return test;}
      template <class F, class C> inline test_args<C> operator-(test_args<C>&& test, fuzz_t::closure cls) {test.args = cls.val; return test;}
      template <class F, class C> inline test_args<C> operator|(test_args<C> const& test, fuzz_t::closure cls) {test_data<F, C> test_ = test; test_.args = cls.val; return test_;}
      template <class F, class C> inline test_args<C> operator-(test_args<C> const& test, fuzz_t::closure cls) {test_data<F, C> test_ = test; test_.args = cls.val; return test_;}
      constexpr struct inv_t {
        template <class F, class C> test_data<F, set_invert<C>> operator()(test_data<F, C>&& test) {return {std::move(test.fn)};}
        template <class F, class C> test_data<F, set_invert<C>> operator()(test_data<F, C> const& test) {return {test.fn};}
      } invert;
      template <class F, class C> inline test_data<F, set_invert<C>> operator|(test_data<F, C>&& test, inv_t) {return {std::move(test.fn), std::move(test.args), std::move(test.pred)};}
      template <class F, class C> inline test_data<F, set_invert<C>> operator-(test_data<F, C>&& test, inv_t) {return {std::move(test.fn), std::move(test.args), std::move(test.pred)};}
      template <class F, class C> inline test_data<F, set_invert<C>> operator|(test_data<F, C> const& test, inv_t) {return {test.fn, test.args, true, test.pred};}
      template <class F, class C> inline test_data<F, set_invert<C>> operator-(test_data<F, C> const& test, inv_t) {return {test.fn, test.args, true, test.pred};}
      template <class C> inline test_args<set_invert<C>> operator|(test_args<C>&& test, inv_t) {return {std::move(test.args)};}
      template <class C> inline test_args<set_invert<C>> operator-(test_args<C>&& test, inv_t) {return {std::move(test.args)};}
      template <class C> inline test_args<set_invert<C>> operator|(test_args<C> const& test, inv_t) {return {test.args};}
      template <class C> inline test_args<set_invert<C>> operator-(test_args<C> const& test, inv_t) {return {test.args};}
      template <class E> struct throw_t {
        template <class F, class C> test_data<F, set_ex<C, E>> operator()(test_data<F, C>&& test) const {return {std::move(test.fn), std::move(test.args), std::move(test.pred)};}
        template <class F, class C> test_data<F, set_ex<C, E>> operator()(test_data<F, C> const& test) const {return {test.fn};}
      };
      template <class E> constexpr throw_t<E> throws;
      template <class F, class C, class E> inline test_data<F, set_ex<C, E>> operator|(test_data<F, C>&& test, throw_t<E>) {return {std::move(test.fn), std::move(test.args), std::move(test.pred)};}
      template <class F, class C, class E> inline test_data<F, set_ex<C, E>> operator-(test_data<F, C>&& test, throw_t<E>) {return {std::move(test.fn), std::move(test.args), std::move(test.pred)};}
      template <class F, class C, class E> inline test_data<F, set_ex<C, E>> operator|(test_data<F, C> const& test, throw_t<E>) {return {test.fn, test.args, test.pred};}
      template <class F, class C, class E> inline test_data<F, set_ex<C, E>> operator-(test_data<F, C> const& test, throw_t<E>) {return {test.fn, test.args, test.pred};}
      template <class C, class E> inline test_args<set_ex<C, E>> operator|(test_args<C>&& test, throw_t<E>) {return {std::move(test.args)};}
      template <class C, class E> inline test_args<set_ex<C, E>> operator-(test_args<C>&& test, throw_t<E>) {return {std::move(test.args)};}
      template <class C, class E> inline test_args<set_ex<C, E>> operator|(test_args<C> const& test, throw_t<E>) {return {test.args};}
      template <class C, class E> inline test_args<set_ex<C, E>> operator-(test_args<C> const& test, throw_t<E>) {return {test.args};}
      constexpr struct predicate_t {
        template <class P> struct closure {P pred;};
        template <class P> closure<P> operator()(P&& pred) const {return {std::move(pred)};}
        template <class P, class F, class C> test_data<F, set_predicate<C, P>> operator()(test_data<F, C>&& test, P&& pred) const {return {std::move(test.fn), std::move(test.args), std::move(pred)};}
        template <class P, class F, class C> test_data<F, set_predicate<C, P>> operator()(test_data<F, C> const& test, P&& pred) const {return {test.fn, test.args, std::move(pred)};}
      } predicate;
      template <class P, class F, class C> inline test_data<F, set_predicate<C, P>> operator|(test_data<F, C>&& test, predicate_t::closure<P> cls) {return {std::move(test.fn), std::move(test.args), std::move(cls.pred)};}
      template <class P, class F, class C> inline test_data<F, set_predicate<C, P>> operator-(test_data<F, C>&& test, predicate_t::closure<P> cls) {return {std::move(test.fn), std::move(test.args), std::move(cls.pred)};}
      template <class P, class F, class C> inline test_data<F, set_predicate<C, P>> operator|(test_data<F, C> const& test, predicate_t::closure<P> cls) {return {test.fn, test.args, std::move(cls.pred)};}
      template <class P, class F, class C> inline test_data<F, set_predicate<C, P>> operator-(test_data<F, C> const& test, predicate_t::closure<P> cls) {return {test.fn, test.args, std::move(cls.pred)};}
      constexpr struct finish_t {
        template <class F, class C> test_case operator()(test_data<F, C>&& test) const {
          return [test] {
            try {
              if (test.args.index() == 2) {
                typename C::args_type args;
                randgen::generator<typename C::args_type> gen;
                for (std::size_t i = 0; i < std::get<1>(test.args); ++i) {
                  gen(args, {});
                  auto res = test.pred(std::apply(test.fn, args));
                  if (C::invert ? res : !res) return false;
                }
                return std::same_as<typename C::exception, std::nothrow_t>;
              }
              else {
                bool res = test.pred(std::apply(test.fn, std::get<0>(test.args)));
                if constexpr (std::same_as<typename C::exception, std::nothrow_t>) return C::invert ? !res : res;
                else return false;
              }
            }
            catch (typename C::exception) {return true;}
            catch (...) {return false;}
          };
        }
        template <class F, class C> test_case operator()(test_data<F, C> const& test) const {
          return [test] {
            try {
              if (test.args.index() == 2) {
                typename C::args_type args;
                randgen::generator<typename C::args_type> gen;
                for (std::size_t i = 0; i < std::get<1>(test.args); ++i) {
                  gen(args, {});
                  auto res = test.pred(std::apply(test.fn, args));
                  if (C::invert ? res : !res) return false;
                }
                return std::same_as<typename C::exception, std::nothrow_t>;
              }
              else {
                bool res = test.pred(std::apply(test.fn, std::get<0>(test.args)));
                if constexpr (std::same_as<typename C::exception, std::nothrow_t>) return C::invert ? !res : res;
                else return false;
              }
            }
            catch (typename C::exception) {return true;}
            catch (...) {return false;}
          };
        }
      } finish;
      template<class F, class C> inline auto operator|(test_data<F, C>&& test, finish_t) {return finish_t{}(std::move(test));}
      template<class F, class C> inline auto operator-(test_data<F, C>&& test, finish_t) {return finish_t{}(std::move(test));}
      template<class F, class C> inline auto operator|(test_data<F, C> const& test, finish_t) {return finish_t{}(test);}
      template<class F, class C> inline auto operator-(test_data<F, C> const& test, finish_t) {return finish_t{}(test);}
    }
    struct tester {
      std::string name;
      tester(std::string const& name) : name(name) {}
      tester(std::string const& name, test_case&& test) : name(name), tests_(std::move(test)) {}
      tester(std::string const& name, std::vector<tester>&& test) : name(name), tests_(std::move(test)) {}
      tester& add(tester&& test) {
        if (tests_.index()) {
          tester self(name);
          std::swap(self, *this);
          tests_ = std::vector{self, std::move(test)};
        }
        else std::get<0>(tests_).push_back(std::move(test));
        return *this;
      }
      bool operator()(std::ostream& os = std::cout) const {
        auto res = print_impl(os, "");
        if (res.first.size()) {
          os << "failed tests:\n";
          for (auto const& i : res.first) os << i << '\n';
          os.flush();
          return false;
        }
        else {
          os << "all tests passed" << std::endl;
          return true;
        }
      }
    private:
      std::pair<std::vector<std::string>, std::size_t> print_impl(std::ostream& os, std::string const& prefix) const {
        if (tests_.index()) {
          os << prefix << "testing " << name << "... " << std::flush;
          auto res = std::get<1>(tests_)();
          os << (res ? "passed" : "failed") << std::endl;
          return res ? std::pair<std::vector<std::string>, std::size_t>{{}, 1} : std::pair<std::vector<std::string>, std::size_t>{std::vector{name}, 1};
        }
        else {
          std::string pf = "  " + prefix;
          std::pair<std::vector<std::string>, std::size_t> res;
          os << prefix << "testing group " << name << "..." << std::endl;
          for (auto const& i : std::get<0>(tests_)) {
            auto r2 = i.print_impl(os, pf);
            res.first.insert(res.first.end(), r2.first.begin(), r2.first.end());
            res.second += r2.second;
          }
          os << prefix << (res.second - res.first.size()) << " out of " << res.second << " passed" << std::endl;
          return res;
        }
      }
      std::variant<std::vector<tester>, test_case> tests_;
    };
  }
}
#define _MDC_DEF_ELEM(_, T, ELEM) if constexpr(generator<decltype(detail::get_type_t<void T>{}.ELEM), detail::get_rand_t<void T>>::has_inplace) generator<decltype(detail::get_type_t<void T>{}.ELEM), detail::get_rand_t<void T>>{}(val.ELEM, rand); else val.ELEM = generator<decltype(detail::get_type_t<void T>{}.ELEM), detail::get_rand_t<void T>>{}(rand);
#define MDC_DEF_RANDGEN(T, ...) \
template <class R> struct generator<T, R> { \
  T operator()(R rand) { \
    T val; \
    BOOST_PP_LIST_FOR_EACH(_MDC_DEF_ELEM, (T, R), BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__))\
    return val; \
  } \
  void operator()(T& val, R rand) { \
    BOOST_PP_LIST_FOR_EACH(_MDC_DEF_ELEM, (T, R), BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__))\
  } \
};
#endif // TEST_HPP
