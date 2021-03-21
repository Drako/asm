#pragma once

#include "../instruction_helper.hxx"

namespace assembly::instructions {
  /// Move immediate to register.
  template<typename T, std::uint8_t Index, REXRequirement RexReq>
  constexpr Instruction mov(Register <T, Index, RexReq> r, T imm)
  {
    if constexpr (sizeof(T)==1u) {
      return helper::opcode_with_register_and_immediate(0xB0, r, imm);
    }
    else {
      return helper::opcode_with_register_and_immediate(0xB8, r, imm);
    }
  }

  /// Move from register to memory.
  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t SourceIdx, REXRequirement SourceRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction mov(
      Memory <RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest,
      Register <VT, SourceIdx, SourceRexReq> src
  )
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_register_and_memory(0x88, src, dest);
    }
    else {
      return helper::opcode_with_register_and_memory(0x89, src, dest);
    }
  }

  /// Move from memory to register.
  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t SourceIdx, REXRequirement SourceRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction mov(
      Register <VT, SourceIdx, SourceRexReq> dest,
      Memory <RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> src
  )
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_register_and_memory(0x8A, dest, src);
    }
    else {
      return helper::opcode_with_register_and_memory(0x8B, dest, src);
    }
  }

  /// Move from register to register.
  template<
      typename T,
      std::uint8_t DestIdx, REXRequirement DestRexReq,
      std::uint8_t SrcIdx, REXRequirement SrcRexReq
  >
  constexpr Instruction mov(Register <T, DestIdx, DestRexReq> dest, Register <T, SrcIdx, SrcRexReq> src)
  {
    if constexpr (sizeof(T)==1u) {
      return helper::opcode_with_register_and_register(0x88, dest, src);
    }
    else {
      return helper::opcode_with_register_and_register(0x89, dest, src);
    }
  }
}
