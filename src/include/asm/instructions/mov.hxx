#pragma once

#include "../instruction.hxx"
#include "../register.hxx"

namespace assembly::instructions {
  /// Move [imm8] to [r8].
  constexpr Instruction mov(Register<std::uint8_t> r8, std::uint8_t imm8)
  {
    Instruction inst{};
    inst.opcode.opcode[0] = static_cast<std::byte>(0xB0+r8.xreg);
    inst.opcode.opcode_size = 1;
    inst.immediate.type = ImmediateType::Imm8;
    inst.immediate.imm8 = imm8;
    return inst;
  }

  /// Move [imm16] to [r16].
  constexpr Instruction mov(Register<std::uint16_t> r16, std::uint16_t imm16)
  {
    Instruction inst{};
    inst.legacy_prefixes.prefix3 = LegacyPrefix3::OperandSizeOverride;
    inst.opcode.opcode[0] = static_cast<std::byte>(0xB8+r16.xreg);
    inst.opcode.opcode_size = 1;
    inst.immediate.type = ImmediateType::Imm16;
    inst.immediate.imm16 = imm16;
    return inst;
  }

  /// Move [imm32] to [r32].
  constexpr Instruction mov(Register<std::uint32_t> r32, std::uint32_t imm32)
  {
    Instruction inst{};
    inst.opcode.opcode[0] = static_cast<std::byte>(0xB8+r32.xreg);
    inst.opcode.opcode_size = 1;
    inst.immediate.type = ImmediateType::Imm32;
    inst.immediate.imm32 = imm32;
    return inst;
  }

  /// Move [imm64] to [r64].
  constexpr Instruction mov(Register<std::uint64_t> r64, std::uint64_t imm64)
  {
    Instruction inst{};
    inst.opcode.rex_prefix = REXPrefix::W;
    inst.opcode.opcode[0] = static_cast<std::byte>(0xB8+r64.xreg);
    inst.opcode.opcode_size = 1;
    inst.immediate.type = ImmediateType::Imm64;
    inst.immediate.imm64 = imm64;
    return inst;
  }

  /// Move from register to register
  template<typename T>
  constexpr Instruction mov(Register<T> dest, Register<T> src)
  {
    Instruction inst{};
    inst.opcode.opcode_size = 1;

    if (src.need_rex || dest.need_rex) {
      inst.opcode.rex_prefix = REXPrefix::Marker;
    }

    if constexpr (sizeof(T)==1u) {
      inst.opcode.opcode[0] = static_cast<std::byte>(0x88);
    }
    else {
      inst.opcode.opcode[0] = static_cast<std::byte>(0x89);
      if constexpr (sizeof(T)==2u) {
        inst.legacy_prefixes.prefix3 = LegacyPrefix3::OperandSizeOverride;
      }
      else if constexpr (sizeof(T)==8u) {
        inst.opcode.rex_prefix |= REXPrefix::W;
      }
    }

    inst.mod_rm.mod = 0x3;
    inst.mod_rm.reg = src.xreg;
    inst.mod_rm.rm = dest.xreg;
    return inst;
  }
}
