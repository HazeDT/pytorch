#pragma once

#include <torch/csrc/utils/variadic.h>
#include <torch/tensor.h>

#include <cstdint>
#include <type_traits>

namespace torch {
namespace nn {
class Module;
} // namespace nn
} // namespace torch

namespace torch {
namespace detail {
/// Detects if a type T has a forward() method.
template <typename T>
struct has_forward {
  // Declare two types with differing size.
  using yes = int8_t;
  using no = int16_t;

  // Here we declare two functions. The first is only enabled if `&U::forward`
  // is well-formed and returns the `yes` type. In C++, the ellipsis parameter
  // type (`...`) always puts the function at the bottom of overload resolution.
  // This is specified in the standard as: 1) A standard conversion sequence is
  // always better than a user-defined conversion sequence or an ellipsis
  // conversion sequence. 2) A user-defined conversion sequence is always better
  // than an ellipsis conversion sequence This means that if the first overload
  // is viable, it will be preferred over the second as long as we pass any
  // convertible type. The type of `&U::forward` is a pointer type, so we can
  // pass e.g. 0.
  template <typename U>
  static yes test(decltype(&U::forward));
  template <typename U>
  static no test(...);

  // Finally we test statically whether the size of the type returned by the
  // selected overload is the size of the `yes` type.
  static constexpr bool value = (sizeof(test<T>(nullptr)) == sizeof(yes));
};

template <typename Head = void, typename... Tail>
constexpr bool check_not_lvalue_references() {
  return (!std::is_lvalue_reference<Head>::value ||
          std::is_const<typename std::remove_reference<Head>::type>::value) &&
      check_not_lvalue_references<Tail...>();
}

template <>
inline constexpr bool check_not_lvalue_references<void>() {
  return true;
}

/// A type trait whose `value` member is true if `M` derives from `Module`.
template <typename M>
using is_module =
    std::is_base_of<torch::nn::Module, typename std::decay<M>::type>;

template <typename M, typename T = void>
using enable_if_module_t =
    typename std::enable_if<is_module<M>::value, T>::type;
} // namespace detail
} // namespace torch
