#pragma once

#include "../instruction_helper.hxx"

namespace assembly::instructions {
  /// add immediate to al, ax, eax
  template<typename T>
  constexpr auto add_a(Register<T, 0u, REXRequirement::DontCare>, T imm)
  -> std::enable_if_t<!std::is_same_v<T, std::uint64_t>, Instruction>
  {
    if constexpr (sizeof(T)==1u) {
      return helper::opcode_with_immediate(0x04, imm);
    }

    auto inst = helper::opcode_with_immediate(0x05, imm);
    if constexpr (sizeof(T)==2u) {
      inst.legacy_prefixes.prefix3 = LegacyPrefix3::OperandSizeOverride;
    }
    return inst;
  }

  /// add imm32 sign extended to rax
  constexpr Instruction add_a(Register<std::uint64_t, 0u, REXRequirement, std::uint32_t imm32)
  {
    auto inst = helper::opcode_with_immediate(0x05, imm32);
    inst.opcode.rex_prefix |= REXPrefix::W;
    return inst;
  }

  /// Add immediate to memory.
  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr auto add(
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest,
      VT imm
  )
  // the 64 bit takes a 32 bit value and does a sign extend
  -> std::enable_if_t<!std::is_same_v<std::uint64_t, VT>, Instruction>
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_memory_and_immediate(0x80, dest, imm);
    }
    else {
      return helper::opcode_with_memory_and_immediate(0x81, dest, imm);
    }
  }

  /// Add imm32 to m64.
  template<
      typename RT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction add64(
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest,
      std::uint32_t imm32
  )
  {
    auto inst = helper::opcode_with_memory_and_immediate(0x81, dest, imm32);
    inst.opcode.rex_prefix |= REXPrefix::W;
    return inst;
  }

  /// Add immediate to register.
  template<typename T, std::uint8_t Index, REXRequirement RexReq>
  constexpr auto add(Register<T, Index, RexReq> r, T imm)
  // the 64 bit takes a 32 bit value and does a sign extend
  -> std::enable_if_t<!std::is_same_v<std::uint64_t, VT>, Instruction>
  {
    if constexpr (sizeof(T)==1u) {
      return helper::opcode_with_register_and_immediate(0x80, r, imm);
    }
    else {
      return helper::opcode_with_register_and_immediate(0x81, r, imm);
    }
  }

  /// Add imm32 to m64.
  template<std::uint8_t Idx>
  constexpr Instruction add64(
      Register<std::uint64_t, Idx, REXRequirement::DontCare> dest,
      std::uint32_t imm32
  )
  {
    return helper::opcode_with_register_and_immediate(0x81, dest, imm32);
  }

  /// Add register to register.
  template<
      typename T,
      std::uint8_t DestIdx, REXRequirement DestRexReq,
      std::uint8_t SrcIdx, REXRequirement SrcRexReq
  >
  constexpr Instruction add(Register<T, DestIdx, DestRexReq> dest, Register<T, SrcIdx, SrcRexReq> src)
  {
    if constexpr (sizeof(T)==1u) {
      return helper::opcode_with_register_and_register(0x00, dest, src);
    }
    else {
      return helper::opcode_with_register_and_register(0x01, dest, src);
    }
  }

  /// Add register to memory.
  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t SourceIdx, REXRequirement SourceRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction add(
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest,
      Register<VT, SourceIdx, SourceRexReq> src
  )
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_register_and_memory(0x00, src, dest);
    }
    else {
      return helper::opcode_with_register_and_memory(0x01, src, dest);
    }
  }

  /// Add memory to register.
  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t SourceIdx, REXRequirement SourceRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction add(
      Register<VT, SourceIdx, SourceRexReq> dest,
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> src
  )
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_register_and_memory(0x02, dest, src);
    }
    else {
      return helper::opcode_with_register_and_memory(0x03, dest, src);
    }
  }

  /// subtract immediate from al, ax, eax
  template<typename T>
  constexpr auto sub_a(Register<T, 0u, REXRequirement::DontCare>, T imm)
  -> std::enable_if_t<!std::is_same_v<T, std::uint64_t>, Instruction>
  {
    if constexpr (sizeof(T)==1u) {
      return helper::opcode_with_immediate(0x2C, imm);
    }

    auto inst = helper::opcode_with_immediate(0x2D, imm);
    if constexpr (sizeof(T)==2u) {
      inst.legacy_prefixes.prefix3 = LegacyPrefix3::OperandSizeOverride;
    }
    return inst;
  }

  /// subtract imm32 sign extended from rax
  constexpr Instruction sub_a(Register<std::uint64_t, 0u, REXRequirement, std::uint32_t imm32)
  {
    auto inst = helper::opcode_with_immediate(0x2D, imm32);
    inst.opcode.rex_prefix |= REXPrefix::W;
    return inst;
  }

  /// Subtract immediate from memory.
  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr auto sub(
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest,
      VT imm
  )
  // the 64 bit takes a 32 bit value and does a sign extend
  -> std::enable_if_t<!std::is_same_v<std::uint64_t, VT>, Instruction>
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_memory_and_immediate(0x80, dest, imm, 5u);
    }
    else {
      return helper::opcode_with_memory_and_immediate(0x81, dest, imm, 5u);
    }
  }

  /// Subtract imm32 from m64.
  template<
      typename RT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction sub64(
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest,
      std::uint32_t imm32
  )
  {
    auto inst = helper::opcode_with_memory_and_immediate(0x81, dest, imm32, 5u);
    inst.opcode.rex_prefix |= REXPrefix::W;
    return inst;
  }

  /// Subtract immediate from register.
  template<typename T, std::uint8_t Index, REXRequirement RexReq>
  constexpr auto sub(Register<T, Index, RexReq> r, T imm)
  // the 64 bit takes a 32 bit value and does a sign extend
  -> std::enable_if_t<!std::is_same_v<std::uint64_t, VT>, Instruction>
  {
    if constexpr (sizeof(T)==1u) {
      return helper::opcode_with_register_and_immediate(0x80, r, imm, 5u);
    }
    else {
      return helper::opcode_with_register_and_immediate(0x81, r, imm, 5u);
    }
  }

  /// Subtract imm32 from m64.
  template<std::uint8_t Idx>
  constexpr Instruction sub64(
      Register<std::uint64_t, Idx, REXRequirement::DontCare> dest,
      std::uint32_t imm32
  )
  {
    return helper::opcode_with_register_and_immediate(0x81, dest, imm32, 5u);
  }

  /// Subtract register from register.
  template<
      typename T,
      std::uint8_t DestIdx, REXRequirement DestRexReq,
      std::uint8_t SrcIdx, REXRequirement SrcRexReq
  >
  constexpr Instruction sub(Register<T, DestIdx, DestRexReq> dest, Register<T, SrcIdx, SrcRexReq> src)
  {
    if constexpr (sizeof(T)==1u) {
      return helper::opcode_with_register_and_register(0x28, dest, src);
    }
    else {
      return helper::opcode_with_register_and_register(0x29, dest, src);
    }
  }

  /// Subtract register from memory.
  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t SourceIdx, REXRequirement SourceRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction sub(
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest,
      Register<VT, SourceIdx, SourceRexReq> src
  )
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_register_and_memory(0x28, src, dest);
    }
    else {
      return helper::opcode_with_register_and_memory(0x29, src, dest);
    }
  }

  /// Subtract memory from register.
  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t SourceIdx, REXRequirement SourceRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction sub(
      Register<VT, SourceIdx, SourceRexReq> dest,
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> src
  )
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_register_and_memory(0x2A, dest, src);
    }
    else {
      return helper::opcode_with_register_and_memory(0x2B, dest, src);
    }
  }
}
