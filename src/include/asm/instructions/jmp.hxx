#pragma once

#include "../instruction_helper.hxx"

namespace assembly::instructions {
  constexpr Instruction jmp(std::int8_t displacement)
  {
    auto inst = helper::opcode<0xEB>();
    helper::detail::set_displacement(inst, displacement);
    inst.mod_rm.mod = 0;
    return inst;
  }

  constexpr Instruction jmp(std::int32_t displacement)
  {
    auto inst = helper::opcode<0xE9>();
    helper::detail::set_displacement(inst, displacement);
    inst.mod_rm.mod = 0;
    return inst;
  }

  template<
      typename RT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction jmp(Memory <RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest)
  {
    return helper::opcode_with_memory(0xFF, dest, 4u);
  }

  constexpr Instruction jmp_rip(std::int32_t displacement)
  {
    auto inst = helper::opcode_with_memory<0xFF>(addr(registers::RIP{}, displacement), 4u);
    inst.mod_rm.mod = 0;
    return inst;
  }

  template<std::uint8_t Idx>
  constexpr Instruction jmp(Register <std::uint64_t, Idx, REXRequirement::DontCare> r)
  {
    auto inst = helper::opcode<0xFF>();
    inst.mod_rm.mod = 0x3;
    inst.mod_rm.reg = 4;
    helper::detail::set_rm(inst, r, true);
    return inst;
  }

  constexpr Instruction je(std::int32_t displacement)
  {
    auto inst = helper::opcode<0x0F, 0x84>();
    helper::detail::set_displacement(inst, displacement);
    inst.mod_rm.mod = 0;
    return inst;
  }

  constexpr Instruction je(std::int8_t displacement)
  {
    auto inst = helper::opcode<0x74>();
    helper::detail::set_displacement(inst, displacement);
    inst.mod_rm.mod = 0;
    return inst;
  }

  constexpr Instruction jne(std::int32_t displacement)
  {
    auto inst = helper::opcode<0x0F, 0x85>();
    helper::detail::set_displacement(inst, displacement);
    inst.mod_rm.mod = 0;
    return inst;
  }

  constexpr Instruction jne(std::int8_t displacement)
  {
    auto inst = helper::opcode<0x75>();
    helper::detail::set_displacement(inst, displacement);
    inst.mod_rm.mod = 0;
    return inst;
  }
}
