#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <variant>

namespace assembly {
  enum class Prefix1 : std::uint8_t {
    None = 0,
    LOCK = 0xF0,
    REPNE = 0xF2,
    REPNZ = 0xF2,
    REP = 0xF3,
    REPE = 0xF3,
    REPZ = 0xF3,
  };

  enum class Prefix2 : std::uint8_t {
    None = 0,
    CSSegmentOverride = 0x2E,
    SSSegmentOverride = 0x36,
    DSSegmentOverride = 0x3E,
    ESSegmentOverride = 0x26,
    FSSegmentOverride = 0x64,
    GSSegmentOverride = 0x65,
    BranchNotTaken = 0x2E,
    BranchTaken = 0x3E,
  };

  enum class Prefix3 : std::uint8_t {
    None = 0,
    OperandSizeOverride = 0x66,
  };

  enum class Prefix4 : std::uint8_t {
    None = 0,
    AddressSizeOverride = 0x67,
  };

  enum class REX : std::uint8_t {
    None = 0,
    Mask = 0x40,
    W = 0x48,
    R = 0x44,
    X = 0x42,
    B = 0x41,
  };

  constexpr REX operator|(REX const lhs, REX const rhs)
  {
    return static_cast<REX>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
  }

  constexpr REX& operator|=(REX& lhs, REX const rhs)
  {
    return (lhs = lhs | rhs);
  }

  struct Opcode {
    std::array<std::uint8_t, 3u> bytes;
    std::uint8_t size;

    constexpr explicit Opcode(std::uint8_t b1 = 0u)
        :bytes{b1}, size{1u} { }

    constexpr Opcode(std::uint8_t b1, std::uint8_t b2)
        :bytes{b1, b2}, size{2u} { }

    constexpr Opcode(std::uint8_t b1, std::uint8_t b2, std::uint8_t b3)
        :bytes{b1, b2, b3}, size{3u} { }
  };

  struct ModRM {
    std::uint8_t rm: 3;
    std::uint8_t reg: 3;
    std::uint8_t mod: 2;
  };

  struct SIB {
    std::uint8_t base: 3;
    std::uint8_t index: 3;
    std::uint8_t scale: 2;
  };

  struct NoValue {
  };

  using Immediate = std::variant<NoValue, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

  using Displacement = std::variant<NoValue, std::int8_t, std::int32_t>;

  struct Instruction {
    Prefix1 prefix1{Prefix1::None};
    Prefix2 prefix2{Prefix2::None};
    Prefix3 prefix3{Prefix3::None};
    Prefix4 prefix4{Prefix4::None};
    REX rex{REX::None};
    Opcode opcode{};
    std::optional<ModRM> mod_rm;
    std::optional<SIB> sib;
    Immediate immediate;
    Displacement displacement;
  };
}
