#pragma once

#include "../instruction_helper.hxx"

namespace assembly::instructions {
  constexpr Instruction call(std::int32_t displacement)
  {
    auto inst = helper::opcode<0xE8>();
    helper::detail::set_displacement(inst, displacement);
    inst.mod_rm.mod = 0;
    return inst;
  }

  constexpr Instruction call_rip(std::int32_t displacement)
  {
    auto inst = helper::opcode_with_memory<0xFF>(addr(registers::RBP{}, displacement), 2u);
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
    return helper::opcode_with_memory<0xFF>(dest, 2u);
  }

  template<std::uint8_t Idx>
  constexpr Instruction call(Register<std::uint64_t, Idx, REXRequirement::DontCare> r)
  {
    return helper::opcode_with_register<0xFF>(r, 2u);
  }
}
