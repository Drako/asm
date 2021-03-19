#pragma once

#include "../instruction_helper.hxx"

#include <type_traits>

namespace assembly::instructions {
  /// Store effective address in register.
  template<typename VT, typename RT>
  constexpr auto lea(Register<VT> dest, Memory<RT> src)
  -> std::enable_if_t<!std::is_same_v<std::uint8_t, VT>, Instruction>
  {
    return helper::single_byte_opcode(dest, src, 0x8D, 0x8A);
  }
}
