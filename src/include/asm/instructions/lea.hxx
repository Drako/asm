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
      Register<VT, SourceIdx, SourceRexReq> dest,
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> src
  )
  -> std::enable_if_t<!std::is_same_v<std::uint8_t, VT>, Instruction>
  {
    return helper::opcode_with_register_and_memory(0x8D, dest, src);
  }
}
