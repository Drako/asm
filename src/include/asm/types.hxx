#pragma once

#include <cstddef>
#include <cstdint>

#include <type_traits>

namespace assembly::types {
  template<typename Needle, typename... Haystack>
  struct IsOneOf final {
    constexpr static bool const value = (std::is_same_v<Needle, Haystack> || ...);
  };

  template<typename Needle, typename... Haystack>
  constexpr static bool const IsOneOfV = IsOneOf<Needle, Haystack...>::value;

  template<typename T>
  struct SignedType;

  template<>
  struct SignedType<std::uint8_t> final {
    using type = std::int8_t;
  };

  template<>
  struct SignedType<std::uint16_t> final {
    using type = std::int16_t;
  };

  template<>
  struct SignedType<std::uint32_t> final {
    using type = std::int32_t;
  };

  template<>
  struct SignedType<std::uint64_t> final {
    using type = std::int64_t;
  };

  template<typename T>
  using SignedTypeT = typename SignedType<T>::type;
}
