#pragma once

#include "../instruction_helper.hxx"

namespace assembly::instructions {
  /// Multiply low single-precision floating-point by low single-precision floating-point.
  template<
      std::uint8_t DestIndex,
      std::uint8_t SrcIndex
  >
  constexpr Instruction mulss(XMM <DestIndex>, XMM <SrcIndex>)
  {
    auto inst = helper::opcode_with_register_and_register<0x0F, 0x59>(
        Register<std::uint32_t, SrcIndex>{},
        Register<std::uint32_t, DestIndex>{}
    );
    inst.opcode.mandatory_prefix = MandatoryPrefix::Mandatory3;
    return inst;
  }

  /// Multiply low single-precision floating-point by low single-precision floating-point.
  template<
      std::uint8_t DestIndex,
      typename RT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction mulss(
      XMM <DestIndex>,
      Memory <RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> src
  )
  {
    auto inst = helper::opcode_with_register_and_memory<0x0F, 0x59>(
        Register<std::uint32_t, DestIndex>{},
        src
    );
    inst.opcode.mandatory_prefix = MandatoryPrefix::Mandatory3;
    return inst;
  }

  /// Convert one signed doubleword integer to one single-precision floating-point value.
  template<
      std::uint8_t XmmIndex,
      std::uint8_t RegIndex
  >
  constexpr Instruction cvtsi2ss(XMM <XmmIndex>, Register <std::uint32_t, RegIndex> src)
  {
    auto inst = helper::opcode_with_register_and_register<0x0F, 0x2A>(
        src, Register<std::uint32_t, XmmIndex>{}
    );
    inst.opcode.mandatory_prefix = MandatoryPrefix::Mandatory3;
    return inst;
  }

  /// Convert one signed doubleword integer to one single-precision floating-point value.
  template<
      std::uint8_t XmmIndex,
      typename RT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction cvtsi2ss(
      XMM <XmmIndex>,
      Memory <RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> src
  )
  {
    auto inst = helper::opcode_with_register_and_memory<0x0F, 0x2A>(
        Register<std::uint32_t, XmmIndex>{},
        src
    );
    inst.opcode.mandatory_prefix = MandatoryPrefix::Mandatory3;
    return inst;
  }
}
