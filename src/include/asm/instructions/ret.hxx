#pragma once

#include "../instruction.hxx"

namespace assembly::instructions {
  /// Near return to calling procedure.
  constexpr Instruction retn()
  {
    Instruction inst{};
    inst.opcode.opcode[0] = static_cast<std::byte>(0xC3);
    inst.opcode.opcode_size = 1;
    return inst;
  }

  /// Near return to calling procedure and pop [imm16] bytes from stack.
  constexpr Instruction retn(std::uint16_t imm16)
  {
    Instruction inst{};
    inst.opcode.opcode[0] = static_cast<std::byte>(0xC2);
    inst.opcode.opcode_size = 1;
    inst.immediate.type = ImmediateType::Imm16;
    inst.immediate.imm16 = imm16;
    return inst;
  }

  /// Far return to calling procedure.
  constexpr Instruction retf()
  {
    Instruction inst{};
    inst.opcode.opcode[0] = static_cast<std::byte>(0xCB);
    inst.opcode.opcode_size = 1;
    return inst;
  }

  /// Far return to calling procedure and pop [imm16] bytes from stack.
  constexpr Instruction retf(std::uint16_t imm16)
  {
    Instruction inst{};
    inst.opcode.opcode[0] = static_cast<std::byte>(0xCA);
    inst.opcode.opcode_size = 1;
    inst.immediate.type = ImmediateType::Imm16;
    inst.immediate.imm16 = imm16;
    return inst;
  }
}
