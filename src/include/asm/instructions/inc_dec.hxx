#pragma once

#include "../instruction_helper.hxx"

namespace assembly::instructions {
  /// Increment memory.
  template<
      typename VT,
      typename RT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction inc(Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest)
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_memory<0xFE>(dest);
    }
    else {
      return helper::opcode_with_memory<0xFF>(dest);
    }
  }

  /// Increment register.
  template<typename VT, std::uint8_t Idx, REXRequirement RexReq>
  constexpr Instruction inc(Register<VT, Idx, RexReq> dest)
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_register<0xFE>(dest);
    }
    else {
      return helper::opcode_with_register<0xFF>(dest);
    }
  }

  /// Decrement memory.
  template<
      typename VT,
      typename RT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction dec(Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest)
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_memory<0xFE>(dest, 1u);
    }
    else {
      return helper::opcode_with_memory<0xFF>(dest, 1u);
    }
  }

  /// Decrement register.
  template<typename VT, std::uint8_t Idx, REXRequirement RexReq>
  constexpr Instruction dec(Register<VT, Idx, RexReq> dest)
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_register<0xFE>(dest, 1u);
    }
    else {
      return helper::opcode_with_register<0xFF>(dest, 1u);
    }
  }
}
