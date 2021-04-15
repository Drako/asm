#pragma once

#include "../instruction_helper.hxx"

namespace assembly::instructions {
  template<std::uint8_t Idx>
  constexpr Instruction push(Register <std::uint64_t, Idx, REXRequirement::DontCare> r)
  {
    Instruction inst = helper::opcode_plus_register<0x50>(r);
    inst.opcode.rex_prefix = REXPrefix::None;
    return inst;
  }

  template<std::uint8_t Idx>
  constexpr Instruction push(Register <std::uint16_t, Idx, REXRequirement::DontCare> r)
  {
    return helper::opcode_plus_register<0x50>(r);
  }

  template<std::uint8_t Idx>
  constexpr Instruction pop(Register <std::uint64_t, Idx, REXRequirement::DontCare> r)
  {
    Instruction inst = helper::opcode_plus_register<0x58>(r);
    inst.opcode.rex_prefix = REXPrefix::None;
    return inst;
  }

  template<std::uint8_t Idx>
  constexpr Instruction pop(Register <std::uint16_t, Idx, REXRequirement::DontCare> r)
  {
    return helper::opcode_plus_register<0x58>(r);
  }
}
