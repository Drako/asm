#pragma once

#include "instruction.hxx"
#include "memory.hxx"
#include "register.hxx"

#include <variant>

namespace assembly::helper {

  namespace detail {
    template<typename T>
    constexpr auto set_immediate(Instruction& inst, T imm)
    -> std::enable_if_t<types::IsOneOfV<T, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>>
    {
      if constexpr (sizeof(T)==1u) {
        inst.immediate.type = ImmediateType::Imm8;
        inst.immediate.imm8 = imm;
      }
      else if constexpr (sizeof(T)==2u) {
        inst.immediate.type = ImmediateType::Imm16;
        inst.immediate.imm16 = imm;
      }
      if constexpr (sizeof(T)==4u) {
        inst.immediate.type = ImmediateType::Imm32;
        inst.immediate.imm32 = imm;
      }
      else if constexpr (sizeof(T)==8u) {
        inst.immediate.type = ImmediateType::Imm64;
        inst.immediate.imm64 = imm;
      }
    }

    constexpr void set_displacement(
        Instruction& inst,
        std::optional<std::variant<std::int8_t, std::int32_t>> displacement
    )
    {
      inst.mod_rm.mod = 0;
      if (displacement.has_value()) {
        auto const d = displacement.value();
        if (std::holds_alternative<std::int8_t>(d)) {
          inst.mod_rm.mod = 0x1;
          inst.displacement.type = DisplacementType::Disp8;
          inst.displacement.disp8 = std::get<std::int8_t>(d);
        }
        else if (std::holds_alternative<std::int32_t>(d)) {
          inst.mod_rm.mod = 0x2;
          inst.displacement.type = DisplacementType::Disp32;
          inst.displacement.disp32 = std::get<std::int32_t>(d);
        }
      }
    }

    template<
        typename RT,
        std::uint8_t BaseIdx, REXRequirement BaseRexReq,
        std::uint8_t IndexIdx, REXRequirement IndexRexReq
    >
    constexpr void set_scaled_index(
        Instruction& inst,
        Register<RT, BaseIdx, BaseRexReq>,
        Register<RT, IndexIdx, IndexRexReq>,
        IndexScale scale
    )
    {
      inst.sib.base = BaseIdx & 7u;
      if constexpr (BaseIdx & 8u) {
        inst.opcode.rex_prefix |= REXPrefix::B;
      }

      inst.sib.index = IndexIdx & 7u;
      if constexpr (IndexIdx & 8u) {
        inst.opcode.rex_prefix |= REXPrefix::X;
      }

      inst.sib.scale = static_cast<std::uint8_t>(scale);
    }

    template<typename T, std::uint8_t Idx, REXRequirement RexReq>
    constexpr void set_reg(Instruction& inst, Register<T, Idx, RexReq>)
    {
      inst.mod_rm.reg = Idx & 7u;
      if constexpr (Idx & 8u) {
        inst.opcode.rex_prefix |= REXPrefix::R;
      }

      if constexpr (RexReq==REXRequirement::Required) {
        inst.opcode.rex_prefix |= REXPrefix::Marker;
      }

      if constexpr (sizeof(T)==2u) {
        inst.legacy_prefixes.prefix3 = LegacyPrefix3::OperandSizeOverride;
      }
      else if constexpr (sizeof(T)==8u) {
        inst.opcode.rex_prefix |= REXPrefix::W;
      }
    }

    template<typename T, std::uint8_t Idx, REXRequirement RexReq>
    constexpr void set_rm(Instruction& inst, Register<T, Idx, RexReq>)
    {
      inst.mod_rm.rm = Idx & 7u;
      if constexpr (Idx & 8u) {
        inst.opcode.rex_prefix |= REXPrefix::B;
      }
    }
  }

  constexpr Instruction opcode(std::uint8_t opcode)
  {
    Instruction inst{};
    inst.opcode.opcode[0] = static_cast<std::byte>(opcode);
    inst.opcode.opcode_size = 1u;
    return inst;
  }

  template<typename T>
  constexpr Instruction opcode_with_immediate(std::uint8_t opcode, T imm)
  {
    auto inst = helper::opcode(opcode);
    detail::set_immediate(inst, imm);
    return inst;
  }

  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t RIdx, REXRequirement RRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction opcode_with_register_and_memory(
      std::uint8_t opcode,
      Register<VT, RIdx, RRexReq> r,
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> mem
  )
  {
    auto inst = helper::opcode(opcode);
    detail::set_reg(inst, r);

    if constexpr (sizeof(RT)==4u) {
      inst.legacy_prefixes.prefix4 = LegacyPrefix4::AddressSizeOverride;
    }

    if (std::holds_alternative<BaseWithDisplacement<RT, BaseIdx, BaseRexReq>>(mem)) {
      detail::set_rm(inst, Register<RT, BaseIdx, BaseRexReq>{});
      detail::set_displacement(inst, std::get<BaseWithDisplacement<RT, BaseIdx, BaseRexReq>>(mem).displacement);
    }
    else {
      inst.mod_rm.rm = 0x4;
      auto const& bsib = std::get<BaseWithScalableIndex<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq>>(mem);
      detail::set_displacement(inst, bsib.displacement);
      detail::set_scaled_index(inst, bsib.base, bsib.index, bsib.scale);
    }
    return inst;
  }

  // naming of parameters might be misleading
  // dest is not necessarily the destination and src is not necessarily the source
  // src always goes into mod_rm.reg
  // dest always goes into mod_rm.rm
  // the opcode decides the direction
  template<
      typename T,
      std::uint8_t DestIdx, REXRequirement DestRexReq,
      std::uint8_t SrcIdx, REXRequirement SrcRexReq
  >
  constexpr Instruction opcode_with_register_and_register(
      std::uint8_t opcode,
      Register<T, DestIdx, DestRexReq> dest,
      Register<T, SrcIdx, SrcRexReq> src
  )
  {
    static_assert(
        DestRexReq==REXRequirement::DontCare ||
            SrcRexReq==REXRequirement::DontCare ||
            DestRexReq==SrcRexReq
    );

    auto inst = helper::opcode(opcode);
    detail::set_reg(inst, src);
    detail::set_rm(inst, dest);
    inst.mod_rm.mod = 0x3;
    return inst;
  }

  template<typename T, std::uint8_t Index, REXRequirement RexReq>
  constexpr Instruction opcode_with_register_and_immediate(
      std::uint8_t opcode,
      Register<T, Index, RexReq>,
      T imm
  )
  {
    auto inst = helper::opcode(opcode+(Index & 7u));

    if constexpr (RexReq==REXRequirement::Required) {
      inst.opcode.rex_prefix |= REXPrefix::Marker;
    }
    if constexpr (Index & 8u) {
      inst.opcode.rex_prefix |= REXPrefix::B;
    }

    if constexpr(sizeof(T)==2u) {
      inst.legacy_prefixes.prefix3 = LegacyPrefix3::OperandSizeOverride;
    }
    else if constexpr (sizeof(T)==8u) {
      inst.opcode.rex_prefix |= REXPrefix::W;
    }

    detail::set_immediate(inst, imm);
    return inst;
  }
}
