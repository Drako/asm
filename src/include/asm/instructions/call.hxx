#pragma once

#include "../instruction_helper.hxx"

#include <cassert>

namespace assembly::instructions {
  constexpr Instruction call(std::int32_t displacement)
  {
    constexpr auto const instruction_size = 5; // opcode + disp32

    assert(displacement>=std::numeric_limits<std::int32_t>::min()+instruction_size);
    displacement -= instruction_size;

    auto inst = helper::opcode(0xE8);
    helper::detail::set_displacement(inst, displacement);
    inst.mod_rm.mod = 0;
    return inst;
  }

  template<
      typename RT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction call(Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest)
  {
    return helper::opcode_with_memory(0xFF, dest, 2u);
  }
}
