#pragma once

#include "../instruction_helper.hxx"

namespace assembly::instructions {
  /// Store effective address in register.
  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t SourceIdx, REXRequirement SourceRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr auto lea(
      Register <VT, SourceIdx, SourceRexReq> dest,
      Memory <RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> src
  )
  -> std::enable_if_t<!std::is_same_v<std::uint8_t, VT>, Instruction>
  {
    return helper::opcode_with_register_and_memory<0x8D>(dest, src);
  }

  /// Store absolute address in register.
  template<std::uint8_t Idx>
  constexpr Instruction lea(
      Register <std::uint64_t, Idx, REXRequirement::DontCare> r,
      std::int32_t disp
  )
  {
    return helper::opcode_with_register_and_memory<0x8D>(r, addr(disp));
  }

  /// Store absolute address in register.
  template<std::uint8_t Idx>
  constexpr Instruction lea_rip(
      Register <std::uint64_t, Idx, REXRequirement::DontCare> r,
      std::int32_t disp
  )
  {
    auto inst = helper::opcode_with_register_and_memory<0x8D>(r, addr(registers::RIP{}, disp));
    inst.mod_rm.mod = 0;
    return inst;
  }
}
