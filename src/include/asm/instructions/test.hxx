#pragma once

#include "../instruction_helper.hxx"

namespace assembly::instructions {
  template<typename T>
  constexpr auto test_a(Register<T, 0u>, T imm)
  -> std::enable_if_t<!std::is_same_v<T, std::uint64_t>, Instruction>
  {
    if constexpr (sizeof(T)==1u) {
      return helper::opcode_with_immediate<0xA8>(imm);
    }

    auto inst = helper::opcode_with_immediate<0xA9>(imm);
    if constexpr (sizeof(T)==2u) {
      inst.legacy_prefixes.prefix3 = LegacyPrefix3::OperandSizeOverride;
    }
    return inst;
  }

  constexpr Instruction test_a(Register<std::uint64_t, 0u>, std::uint32_t imm32)
  {
    auto inst = helper::opcode_with_immediate<0xA9>(imm32);
    inst.opcode.rex_prefix |= REXPrefix::W;
    return inst;
  }

  template<
      typename VT,
      typename RT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr auto test(
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest,
      VT imm
  )
  // the 64 bit takes a 32 bit value and does a sign extend
  -> std::enable_if_t<!std::is_same_v<std::uint64_t, VT>, Instruction>
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_memory_and_immediate<0xF6>(dest, imm);
    }
    else {
      return helper::opcode_with_memory_and_immediate<0xF7>(dest, imm);
    }
  }

  template<
      typename RT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction test64(
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest,
      std::uint32_t imm32
  )
  {
    auto inst = helper::opcode_with_memory_and_immediate<0xF7>(dest, imm32);
    inst.opcode.rex_prefix |= REXPrefix::W;
    return inst;
  }

  template<typename T, std::uint8_t Index, REXRequirement RexReq>
  constexpr auto test(Register<T, Index, RexReq> r, T imm)
  // the 64 bit takes a 32 bit value and does a sign extend
  -> std::enable_if_t<!std::is_same_v<std::uint64_t, T>, Instruction>
  {
    if constexpr (sizeof(T)==1u) {
      return helper::opcode_with_register_and_immediate<0xF6>(r, imm);
    }
    else {
      return helper::opcode_with_register_and_immediate<0xF7>(r, imm);
    }
  }

  template<std::uint8_t Idx>
  constexpr Instruction test64(
      Register<std::uint64_t, Idx> dest,
      std::uint32_t imm32
  )
  {
    return helper::opcode_with_register_and_immediate<0xF7>(dest, imm32);
  }

  template<
      typename T,
      std::uint8_t DestIdx, REXRequirement DestRexReq,
      std::uint8_t SrcIdx, REXRequirement SrcRexReq
  >
  constexpr Instruction test(Register<T, DestIdx, DestRexReq> dest, Register<T, SrcIdx, SrcRexReq> src)
  {
    if constexpr (sizeof(T)==1u) {
      return helper::opcode_with_register_and_register<0x84>(dest, src);
    }
    else {
      return helper::opcode_with_register_and_register<0x85>(dest, src);
    }
  }

  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t SourceIdx, REXRequirement SourceRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction test(
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> dest,
      Register<VT, SourceIdx, SourceRexReq> src
  )
  {
    if constexpr (sizeof(VT)==1u) {
      return helper::opcode_with_register_and_memory<0x84>(src, dest);
    }
    else {
      return helper::opcode_with_register_and_memory<0x85>(src, dest);
    }
  }
}
