#pragma once

#include "instruction.hxx"
#include "memory.hxx"
#include "register.hxx"

namespace assembly::helper {
  template<typename RT, typename VT>
  constexpr Instruction single_byte_opcode(
      Memory<RT> dest,
      Register<VT> src,
      std::uint8_t opcode,
      std::uint8_t opcode8
  )
  {
    Instruction inst{};
    if (src.need_rex) {
      inst.opcode.rex_prefix = REXPrefix::Marker;
    }

    if constexpr (sizeof(VT)==8u) {
      inst.opcode.rex_prefix |= REXPrefix::W;
    }

    if constexpr (sizeof(RT)==4u) {
      inst.legacy_prefixes.prefix4 = LegacyPrefix4::AddressSizeOverride;
    }

    detail::handle_single_byte_opcode<VT>(inst, opcode, opcode8);

    if (dest.index.has_value()) {
      inst.mod_rm.rm = 0x4;
      detail::handle_displacement(inst, dest);
      detail::handle_scaled_index(inst, dest);
    }
    else {
      inst.mod_rm.rm = dest.base.xreg;
      detail::handle_displacement(inst, dest);
    }
    inst.mod_rm.reg = src.xreg;

    return inst;
  }

  template<typename VT, typename RT>
  constexpr Instruction single_byte_opcode(
      Register<VT> dest,
      Memory<RT> src,
      std::uint8_t opcode,
      std::uint8_t opcode8
  )
  {
    return single_byte_opcode(src, dest, opcode, opcode8);
  }

  template<typename T>
  constexpr Instruction single_byte_opcode(
      Register<T> dest,
      Register<T> src,
      std::uint8_t opcode,
      std::uint8_t opcode8
  )
  {
    Instruction inst{};
    inst.opcode.opcode_size = 1;

    if (src.need_rex || dest.need_rex) {
      inst.opcode.rex_prefix = REXPrefix::Marker;
    }

    if constexpr (sizeof(T)==4u) {
      inst.legacy_prefixes.prefix4 = LegacyPrefix4::AddressSizeOverride;
    }

    detail::handle_single_byte_opcode<T>(inst, opcode, opcode8);

    inst.mod_rm.mod = 0x3;
    inst.mod_rm.reg = src.xreg;
    inst.mod_rm.rm = dest.xreg;
    return inst;
  }
}
