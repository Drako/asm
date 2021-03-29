#pragma once

#include "instruction.hxx"
#include "register.hxx"

#include <cassert>

#include <optional>
#include <type_traits>
#include <variant>

namespace assembly {
  enum class IndexScale {
    Scale1 = 0u,
    Scale2 = 1u,
    Scale4 = 2u,
    Scale8 = 3u,
  };

  template<typename T, std::uint8_t Idx, REXRequirement RexReq>
  struct BaseWithDisplacement final {
    static_assert(sizeof(T)==4u || sizeof(T)==8u);

    using base = Register<T, Idx, RexReq>;
    std::optional<std::variant<std::int8_t, std::int32_t>> displacement{};
  };

  template<
      typename T,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx, REXRequirement IndexRexReq
  >
  struct BaseWithScalableIndex final {
    static_assert(sizeof(T)==4u || sizeof(T)==8u);

    constexpr static Register<T, BaseIdx, BaseRexReq> const base{};
    constexpr static Register<T, IndexIdx, IndexRexReq> const index{};
    IndexScale scale = IndexScale::Scale1;
    std::optional<std::variant<std::int8_t, std::int32_t>> displacement{};
  };

  template<
      typename T,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx, REXRequirement IndexRexReq
  >
  using Memory = std::variant<
      BaseWithDisplacement<T, BaseIdx, BaseRexReq>,
      BaseWithScalableIndex<T, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq>
  >;

  template<typename T, std::uint8_t Idx, REXRequirement RexReq>
  constexpr Memory<T, Idx, RexReq, 0u, REXRequirement::DontCare> addr(
      Register<T, Idx, RexReq>,
      std::optional<std::variant<std::int8_t, std::int32_t>> displacement = {}
  )
  {
    return BaseWithDisplacement<T, Idx, RexReq>{displacement};
  }

  template<
      typename T,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx, REXRequirement IndexRexReq
  >
  constexpr Memory<T, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> addr(
      Register<T, BaseIdx, BaseRexReq>,
      Register<T, IndexIdx, IndexRexReq>,
      IndexScale scale = IndexScale::Scale1,
      std::optional<std::variant<std::int8_t, std::int32_t>> displacement = {}
  )
  {
    return BaseWithScalableIndex<T, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq>{scale, displacement};
  }

  constexpr auto addr(std::int32_t displacement)
  {
    return addr(registers::RBP{}, registers::RSP{}, IndexScale::Scale1, displacement);
  }
}
