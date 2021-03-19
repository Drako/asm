#pragma once

#include "instruction.hxx"
#include "register.hxx"

#include <cassert>

#include <optional>
#include <type_traits>
#include <variant>

namespace assembly {
  template<typename RegisterType>
  struct Memory final {
    using register_type = RegisterType;

    Register<register_type> base;
    std::optional<Register<register_type>> index{};
    std::uint8_t scale = 1u; // only 1, 2, 4 and 8 are allowed
    std::optional<std::variant<std::int8_t, std::int32_t>> displacement{};
  };

  namespace detail {
    template<typename RegisterType>
    constexpr void handle_displacement(Instruction& inst, Memory<RegisterType> const& mem)
    {
      if (mem.displacement.has_value()) {
        auto const displacement = mem.displacement.value();
        if (std::holds_alternative<std::int8_t>(displacement)) {
          inst.mod_rm.mod = 0x1;
          inst.displacement.type = DisplacementType::Disp8;
          inst.displacement.disp8 = std::get<std::int8_t>(displacement);
        }
        else if (std::holds_alternative<std::int32_t>(displacement)) {
          inst.mod_rm.mod = 0x2;
          inst.displacement.type = DisplacementType::Disp32;
          inst.displacement.disp32 = std::get<std::int32_t>(displacement);
        }
      }
      else {
        inst.mod_rm.mod = 0x0;
      }
    }

    template<typename RegisterType>
    constexpr void handle_scaled_index(Instruction& inst, Memory<RegisterType> const& mem)
    {
      assert(mem.index.has_value());
      inst.sib.base = mem.base.xreg;
      inst.sib.index = mem.index.value().xreg;
      switch (mem.scale) {
      case 1u:
        inst.sib.scale = 0x0;
        break;
      case 2u:
        inst.sib.scale = 0x1;
        break;
      case 4u:
        inst.sib.scale = 0x2;
        break;
      case 8u:
        inst.sib.scale = 0x3;
        break;
      default:
        assert(false);
      }
    }
  }

  template<typename RegisterType>
  constexpr auto addr(
      Register<RegisterType> base,
      std::optional<std::variant<std::int8_t, std::int32_t>> displacement = {}
  )
  -> std::enable_if_t<
      std::is_same_v<std::uint32_t, RegisterType> || std::is_same_v<std::uint64_t, RegisterType>, Memory<RegisterType>
  >
  {
    return {base, {}, 1u, displacement};
  }

  template<typename RegisterType>
  constexpr auto addr(
      Register<RegisterType> base,
      Register<RegisterType> index,
      std::uint8_t scale = 1u,
      std::optional<std::variant<std::int8_t, std::int32_t>> displacement = {}
  )
  -> std::enable_if_t<
      std::is_same_v<std::uint32_t, RegisterType> || std::is_same_v<std::uint64_t, RegisterType>, Memory<RegisterType>
  >
  {
    return {base, index, scale, displacement};
  }
}
