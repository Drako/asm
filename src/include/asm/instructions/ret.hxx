#pragma once

#include "../instruction_helper.hxx"

namespace assembly::instructions {
  /// Near return to calling procedure.
  constexpr Instruction retn()
  {
    return helper::opcode<0xC3>();
  }

  /// Near return to calling procedure and pop [imm16] bytes from stack.
  constexpr Instruction retn(std::uint16_t imm16)
  {
    return helper::opcode_with_immediate<0xC2>(imm16);
  }

  /// Far return to calling procedure.
  constexpr Instruction retf()
  {
    return helper::opcode<0xCB>();
  }

  /// Far return to calling procedure and pop [imm16] bytes from stack.
  constexpr Instruction retf(std::uint16_t imm16)
  {
    return helper::opcode_with_immediate<0xCA>(imm16);
  }
}
