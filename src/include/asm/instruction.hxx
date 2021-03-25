#pragma once

#include <cstddef>
#include <cstdint>

#include <array>

namespace assembly {
  enum class LegacyPrefix1 : std::uint8_t {
    None = 0,
    Lock = 0xF0,
    Repne = 0xF2,
    Repnz = 0xF2,
    Rep = 0xF3,
    Repe = 0xF3,
    Repz = 0xF3
  };

  enum class LegacyPrefix2 : std::uint8_t {
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

  enum class LegacyPrefix3 : std::uint8_t {
    None = 0,
    OperandSizeOverride = 0x66,
  };

  enum class LegacyPrefix4 : std::uint8_t {
    None = 0,
    AddressSizeOverride = 0x67,
  };

  struct LegacyPrefixes final {
    LegacyPrefix1 prefix1 = LegacyPrefix1::None;
    LegacyPrefix2 prefix2 = LegacyPrefix2::None;
    LegacyPrefix3 prefix3 = LegacyPrefix3::None;
    LegacyPrefix4 prefix4 = LegacyPrefix4::None;
  };

  enum class MandatoryPrefix : std::uint8_t {
    None = 0,
    Mandatory1 = 0x66,
    Mandatory2 = 0xF2,
    Mandatory3 = 0xF3,
  };

  enum class REXPrefix : std::uint8_t {
    None = 0,
    Marker = 0x40,
    W = 0x08,
    R = 0x04,
    X = 0x02,
    B = 0x01,
  };

  constexpr REXPrefix operator|(REXPrefix lhs, REXPrefix rhs)
  {
    return static_cast<REXPrefix>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
  }

  constexpr REXPrefix& operator|=(REXPrefix& lhs, REXPrefix rhs)
  {
    lhs = static_cast<REXPrefix>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
    return lhs;
  }

  struct LegacyOpcode final {
    MandatoryPrefix mandatory_prefix = MandatoryPrefix::None;
    REXPrefix rex_prefix = REXPrefix::None;
    std::byte opcode[3] = {}; // 3 is max size, actual size is in opcode_size
    std::size_t opcode_size = 0u;
  };

  union ModRM final {
    struct {
      std::uint8_t rm: 3;
      std::uint8_t reg: 3;
      std::uint8_t mod: 2;
    };
    std::uint8_t value = 0u;
  };

  union SIB final {
    struct {
      std::uint8_t base: 3;
      std::uint8_t index: 3;
      std::uint8_t scale: 2;
    };
    std::uint8_t value = 0u;
  };

  enum class ImmediateType {
    None = 0,
    Imm8,
    Imm16,
    Imm32,
    Imm64,
  };

  struct Immediate final {
    ImmediateType type = ImmediateType::None;
    union {
      std::uint8_t imm8;
      std::uint16_t imm16;
      std::uint32_t imm32;
      std::uint64_t imm64;
      std::byte bytes[8];
    };
  };

  enum class DisplacementType {
    None = 0,
    Disp8,
    Disp16,
    Disp32,
    Disp64,
  };

  struct Displacement final {
    DisplacementType type = DisplacementType::None;
    union {
      std::int8_t disp8;
      std::int16_t disp16;
      std::int32_t disp32;
      std::int64_t disp64;
      std::byte bytes[8];
    };
  };

  struct Instruction final {
    LegacyPrefixes legacy_prefixes{};
    LegacyOpcode opcode{};
    ModRM mod_rm{};
    SIB sib{};
    Displacement displacement{};
    Immediate immediate{};

    constexpr std::size_t size() const
    {
      std::size_t bytes = 0u;
      if (legacy_prefixes.prefix1!=LegacyPrefix1::None) {
        ++bytes;
      }
      if (legacy_prefixes.prefix2!=LegacyPrefix2::None) {
        ++bytes;
      }
      if (legacy_prefixes.prefix3!=LegacyPrefix3::None) {
        ++bytes;
      }
      if (legacy_prefixes.prefix4!=LegacyPrefix4::None) {
        ++bytes;
      }

      if (opcode.mandatory_prefix!=MandatoryPrefix::None) {
        ++bytes;
      }
      if (opcode.rex_prefix!=REXPrefix::None) {
        ++bytes;
      }
      bytes += opcode.opcode_size;

      if (mod_rm.value!=0u) {
        ++bytes;
      }
      if (sib.value!=0u) {
        ++bytes;
      }

      switch (displacement.type) {
      default:
        break;
      case DisplacementType::Disp8:
        ++bytes;
        break;
      case DisplacementType::Disp16:
        bytes += 2;
        break;
      case DisplacementType::Disp32:
        bytes += 4;
        break;
      case DisplacementType::Disp64:
        bytes += 8;
        break;
      }

      switch (immediate.type) {
      default:
        break;
      case ImmediateType::Imm8:
        ++bytes;
        break;
      case ImmediateType::Imm16:
        bytes += 2;
        break;
      case ImmediateType::Imm32:
        bytes += 4;
        break;
      case ImmediateType::Imm64:
        bytes += 8;
        break;
      }

      return bytes;
    }
  };
}
