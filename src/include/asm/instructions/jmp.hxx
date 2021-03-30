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

#define ASM_JCC(suffix, opcode8) \
  constexpr Instruction j##suffix(std::int32_t displacement) \
  { \
    auto inst = helper::opcode<0x0F, opcode8+0x10>(); \
    helper::detail::set_displacement(inst, displacement); \
    inst.mod_rm.mod = 0; \
    return inst; \
  } \
  constexpr Instruction j##suffix(std::int8_t displacement) \
  { \
    auto inst = helper::opcode<opcode8>(); \
    helper::detail::set_displacement(inst, displacement); \
    inst.mod_rm.mod = 0; \
    return inst; \
  }

  /// Jump short if above (CF=0 and ZF=0).
  ASM_JCC(a, 0x77)

  /// Jump short if above or equal (CF=0).
  ASM_JCC(ae, 0x73)

  /// Jump short if below (CF=1).
  ASM_JCC(b, 0x72)

  /// Jump short if below or equal (CF=1 or ZF=1).
  ASM_JCC(be, 0x76)

  /// Jump short if carry (CF=1).
  ASM_JCC(c, 0x72)

  /// Jump short if equal (ZF=1).
  ASM_JCC(e, 0x74)

  /// Jump short if greater (ZF=0 and SF=OF).
  ASM_JCC(g, 0x7F)

  /// Jump short if greater or equal (SF=OF).
  ASM_JCC(ge, 0x7D)

  /// Jump short if less (SF≠ OF).
  ASM_JCC(l, 0x7C)

  /// Jump short if less or equal (ZF=1 or SF≠OF).
  ASM_JCC(le, 0x7E)

  /// Jump short if not above (CF=1 or ZF=1).
  ASM_JCC(na, 0x76)

  /// Jump short if not above or equal (CF=1).
  ASM_JCC(nae, 0x72)

  /// Jump short if not below (CF=0).
  ASM_JCC(nb, 0x73)

  /// Jump short if not below or equal (CF=0 and ZF=0).
  ASM_JCC(nbe, 0x77)

  /// Jump short if not carry (CF=0).
  ASM_JCC(nc, 0x73)

  /// Jump short if not equal (ZF=0).
  ASM_JCC(ne, 0x75)

  /// Jump short if not greater (ZF=1 or SF≠OF).
  ASM_JCC(ng, 0x7E)

  /// Jump short if not greater or equal (SF≠OF).
  ASM_JCC(nge, 0x7C)

  /// Jump short if not less (SF=OF).
  ASM_JCC(nl, 0x7D)

  /// Jump short if not less or equal (ZF=0 and SF=OF).
  ASM_JCC(nle, 0x7F)

  /// Jump short if not overflow (OF=0).
  ASM_JCC(no, 0x71)

  /// Jump short if not parity (PF=0).
  ASM_JCC(np, 0x7B)

  /// Jump short if not sign (SF=0).
  ASM_JCC(ns, 0x79)

  /// Jump short if not zero (ZF=0).
  ASM_JCC(nz, 0x75)

  /// Jump short if overflow (OF=1).
  ASM_JCC(o, 0x70)

  /// Jump short if parity (PF=1).
  ASM_JCC(p, 0x7A)

  /// Jump short if parity even (PF=1).
  ASM_JCC(pe, 0x7A)

  /// Jump short if parity odd (PF=0).
  ASM_JCC(po, 0x7B)

  /// Jump short if sign (SF=1).
  ASM_JCC(s, 0x78)

  /// Jump short if zero (ZF=1).
  ASM_JCC(z, 0x74)

#undef ASM_JCC

  /// Jump short if ECX register is 0.
  constexpr Instruction jecxz(std::int8_t displacement)
  {
    auto inst = helper::opcode<0xE3>();
    inst.legacy_prefixes.prefix4 = LegacyPrefix4::AddressSizeOverride;
    helper::detail::set_displacement(inst, displacement);
    inst.mod_rm.mod = 0;
    return inst;
  }

  /// Jump short if RCX register is 0.
  constexpr Instruction jrcxz(std::int8_t displacement)
  {
    auto inst = helper::opcode<0xE3>();
    helper::detail::set_displacement(inst, displacement);
    inst.mod_rm.mod = 0;
    return inst;
  }
}
