#pragma once

#include "../instruction_helper.hxx"

namespace assembly::instructions {
  /// Set RSP to RBP, then pop RBP.
  constexpr Instruction leave()
  {
    return helper::opcode(0xC9);
  }
}
