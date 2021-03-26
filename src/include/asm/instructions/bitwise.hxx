#pragma once

#include "../instruction_helper.hxx"

namespace assembly::instructions {
  template<typename T>
  constexpr auto xor_a(Register<T, 0u, REXRequirement::DontCare>, T imm)
  -> std::enable_if_t<!std::is_same_v<T, std::uint64_t>, Instruction>
  {
    if constexpr (sizeof(T)==1u) {
      return helper::opcode_with_immediate<0x34>(imm);
    }

    auto inst = helper::opcode_with_immediate<0x35>(imm);
    if constexpr (sizeof(T)==2u) {
      inst.legacy_prefixes.prefix3 = LegacyPrefix3::OperandSizeOverride;
    }
    return inst;
  }

  constexpr Instruction xor_a(Register<std::uint64_t, 0u, REXRequirement::DontCare>, std::uint32_t imm32)
  {
    auto inst = helper::opcode_with_immediate<0x35>(imm32);
    inst.opcode.rex_prefix |= REXPrefix::W;
    return inst;
  }

  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr auto xor_(
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest,
      VT imm
  )
  // the 64 bit takes a 32 bit value and does a sign extend
  -> std::enable_if_t<!std::is_same_v<std::uint64_t, VT>, Instruction>
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_memory_and_immediate<0x80>(dest, imm, 6u);
    }
    else {
      return helper::opcode_with_memory_and_immediate<0x81>(dest, imm, 6u);
    }
  }

  template<
      typename RT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction xor64(
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest,
      std::uint32_t imm32
  )
  {
    auto inst = helper::opcode_with_memory_and_immediate<0x81>(dest, imm32, 6u);
    inst.opcode.rex_prefix |= REXPrefix::W;
    return inst;
  }

  template<typename T, std::uint8_t Index, REXRequirement RexReq>
  constexpr auto xor_(Register<T, Index, RexReq> r, T imm)
  // the 64 bit takes a 32 bit value and does a sign extend
  -> std::enable_if_t<!std::is_same_v<std::uint64_t, T>, Instruction>
  {
    if constexpr (sizeof(T)==1u) {
      return helper::opcode_with_register_and_immediate<0x80>(r, imm, 6u);
    }
    else {
      return helper::opcode_with_register_and_immediate<0x81>(r, imm, 6u);
    }
  }

  template<std::uint8_t Idx>
  constexpr Instruction xor64(
      Register<std::uint64_t, Idx, REXRequirement::DontCare> dest,
      std::uint32_t imm32
  )
  {
    return helper::opcode_with_register_and_immediate<0x81>(dest, imm32, 6u);
  }

  template<
      typename T,
      std::uint8_t DestIdx, REXRequirement DestRexReq,
      std::uint8_t SrcIdx, REXRequirement SrcRexReq
  >
  constexpr Instruction xor_(Register<T, DestIdx, DestRexReq> dest, Register<T, SrcIdx, SrcRexReq> src)
  {
    if constexpr (sizeof(T)==1u) {
      return helper::opcode_with_register_and_register<0x30>(dest, src);
    }
    else {
      return helper::opcode_with_register_and_register<0x31>(dest, src);
    }
  }

  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t SourceIdx, REXRequirement SourceRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction xor_(
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest,
      Register<VT, SourceIdx, SourceRexReq> src
  )
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_register_and_memory<0x30>(src, dest);
    }
    else {
      return helper::opcode_with_register_and_memory<0x31>(src, dest);
    }
  }

  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t SourceIdx, REXRequirement SourceRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction xor_(
      Register<VT, SourceIdx, SourceRexReq> dest,
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> src
  )
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_register_and_memory<0x32>(dest, src);
    }
    else {
      return helper::opcode_with_register_and_memory<0x33>(dest, src);
    }
  }
}
