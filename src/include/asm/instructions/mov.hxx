#pragma once

#include "../instruction_helper.hxx"

namespace assembly::instructions {
  /// Move [imm8] to [r8].
  constexpr Instruction mov(Register <std::uint8_t> r8, std::uint8_t imm8)
  {
    Instruction inst{};
    if (r8.need_rex) {
      inst.opcode.rex_prefix = REXPrefix::Marker;
    }
    inst.opcode.opcode[0] = static_cast<std::byte>(0xB0+r8.xreg);
    inst.opcode.opcode_size = 1;
    inst.immediate.type = ImmediateType::Imm8;
    inst.immediate.imm8 = imm8;
    return inst;
  }

  /// Move [imm16] to [r16].
  constexpr Instruction mov(Register <std::uint16_t> r16, std::uint16_t imm16)
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
  constexpr Instruction mov(Register <std::uint32_t> r32, std::uint32_t imm32)
  {
    Instruction inst{};
    inst.opcode.opcode[0] = static_cast<std::byte>(0xB8+r32.xreg);
    inst.opcode.opcode_size = 1;
    inst.immediate.type = ImmediateType::Imm32;
    inst.immediate.imm32 = imm32;
    return inst;
  }

  /// Move [imm64] to [r64].
  constexpr Instruction mov(Register <std::uint64_t> r64, std::uint64_t imm64)
  {
    Instruction inst{};
    inst.opcode.rex_prefix = REXPrefix::W;
    inst.opcode.opcode[0] = static_cast<std::byte>(0xB8+r64.xreg);
    inst.opcode.opcode_size = 1;
    inst.immediate.type = ImmediateType::Imm64;
    inst.immediate.imm64 = imm64;
    return inst;
  }

  /// Move from register to memory.
  template<typename RT, typename VT>
  constexpr Instruction mov(Memory <RT> dest, Register <VT> src)
  {
    return helper::single_byte_opcode(dest, src, 0x89, 0x88);
  }

  /// Move from memory to register.
  template<typename VT, typename RT>
  constexpr Instruction mov(Register <VT> dest, Memory <RT> src)
  {
    return helper::single_byte_opcode(dest, src, 0x8B, 0x8A);
  }

  /// Move from register to register.
  template<typename T>
  constexpr Instruction mov(Register <T> dest, Register <T> src)
  {
    return helper::single_byte_opcode(dest, src, 0x89, 0x88);
  }
}
