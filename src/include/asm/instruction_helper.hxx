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
      SIB sib{};

      sib.base = BaseIdx & 7u;
      if constexpr ((BaseIdx & 8u)!=0u) {
        inst.opcode.rex_prefix |= REXPrefix::B;
      }

      sib.index = IndexIdx & 7u;
      if constexpr ((IndexIdx & 8u)!=0u) {
        inst.opcode.rex_prefix |= REXPrefix::X;
      }

      sib.scale = static_cast<std::uint8_t>(scale);

      inst.sib = sib;
    }

    template<typename T, std::uint8_t Idx, REXRequirement RexReq>
    constexpr void set_reg(Instruction& inst, Register<T, Idx, RexReq>)
    {
      inst.mod_rm.reg = Idx & 7u;
      if constexpr ((Idx & 8u)!=0u) {
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
    constexpr void set_rm(Instruction& inst, Register<T, Idx, RexReq>, bool is_address = false)
    {
      inst.mod_rm.rm = Idx & 7u;
      if constexpr ((Idx & 8u)!=0u) {
        inst.opcode.rex_prefix |= REXPrefix::B;
      }

      if (!is_address) {
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
    }
  }

  template<std::uint8_t... Opcode>
  constexpr Instruction opcode()
  {
    static_assert(sizeof...(Opcode)>=1 && sizeof...(Opcode)<=3u);
    std::byte opc[] = {static_cast<std::byte>(Opcode)...};

    Instruction inst{};
    for (std::size_t n = 0; n<sizeof...(Opcode); ++n) {
      inst.opcode.opcode[n] = opc[n];
    }
    inst.opcode.opcode_size = sizeof...(Opcode);
    return inst;
  }

  template<std::uint8_t... Opcode, typename T>
  constexpr Instruction opcode_with_immediate(T imm)
  {
    auto inst = helper::opcode<Opcode...>();
    detail::set_immediate(inst, imm);
    return inst;
  }

  template<
      std::uint8_t... Opcode,
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t RIdx, REXRequirement RRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction opcode_with_register_and_memory(
      Register<VT, RIdx, RRexReq> r,
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> mem
  )
  {
    auto inst = helper::opcode<Opcode...>();
    detail::set_reg(inst, r);

    if constexpr (sizeof(RT)==4u) {
      inst.legacy_prefixes.prefix4 = LegacyPrefix4::AddressSizeOverride;
    }

    if (std::holds_alternative<BaseWithDisplacement<RT, BaseIdx, BaseRexReq>>(mem)) {
      detail::set_rm(inst, Register<RT, BaseIdx, BaseRexReq>{}, true);
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

  template<
      std::uint8_t... Opcode,
      typename T,
      std::uint8_t DestIdx, REXRequirement DestRexReq,
      std::uint8_t SrcIdx, REXRequirement SrcRexReq
  >
  constexpr Instruction opcode_with_register_and_register(
      Register<T, DestIdx, DestRexReq> rm,
      Register<T, SrcIdx, SrcRexReq> reg
  )
  {
    static_assert(
        DestRexReq==REXRequirement::DontCare ||
            SrcRexReq==REXRequirement::DontCare ||
            DestRexReq==SrcRexReq
    );

    auto inst = helper::opcode<Opcode...>();
    detail::set_reg(inst, reg);
    detail::set_rm(inst, rm);
    inst.mod_rm.mod = 0x3;
    return inst;
  }

  template<std::uint8_t... Opcode, typename VT, std::uint8_t Idx, REXRequirement RexReq>
  constexpr Instruction opcode_with_register(
      Register<VT, Idx, RexReq> r,
      std::uint8_t reg = 0u
  )
  {
    auto inst = helper::opcode<Opcode...>();
    inst.mod_rm.mod = 0x3;
    inst.mod_rm.reg = reg;
    detail::set_rm(inst, r);
    return inst;
  }

  template<
      std::uint8_t... Opcode,
      typename RT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction opcode_with_memory(
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> mem,
      std::uint8_t reg = 0u
  )
  {
    auto inst = helper::opcode<Opcode...>();
    inst.mod_rm.reg = reg;

    if constexpr (sizeof(RT)==4u) {
      inst.legacy_prefixes.prefix4 = LegacyPrefix4::AddressSizeOverride;
    }

    if (std::holds_alternative<BaseWithDisplacement<RT, BaseIdx, BaseRexReq>>(mem)) {
      detail::set_rm(inst, Register<RT, BaseIdx, BaseRexReq>{}, true);
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

  template<
      typename RT,
      typename VT,
      std::uint8_t BaseIdx, REXRequirement BaseRexReq,
      std::uint8_t IndexIdx = 0u, REXRequirement IndexRexReq = REXRequirement::DontCare
  >
  constexpr Instruction opcode_with_memory_and_immediate(
      std::uint8_t opcode,
      Memory<RT, BaseIdx, BaseRexReq, IndexIdx, IndexRexReq> mem,
      VT imm,
      std::uint8_t reg = 0u
  )
  {
    auto inst = opcode_with_memory(opcode, mem, reg);
    detail::set_immediate(inst, imm);
    return inst;
  }

  template<std::uint8_t... Opcode, typename VT, typename RT, std::uint8_t Idx, REXRequirement RexReq>
  constexpr Instruction opcode_with_register_and_immediate(
      Register<RT, Idx, RexReq> r,
      VT imm,
      std::uint8_t reg = 0u
  )
  {
    auto inst = opcode_with_register<Opcode...>(r, reg);
    detail::set_immediate(inst, imm);
    return inst;
  }

  template<std::uint8_t Opcode, typename T, std::uint8_t Index, REXRequirement RexReq>
  constexpr Instruction opcode_plus_register_with_immediate(
      Register<T, Index, RexReq>,
      T imm
  )
  {
    // TODO: might need adjustment if there are multibyte opcodes which add the register
    auto inst = helper::opcode<Opcode+(Index & 7u)>();

    if constexpr (RexReq==REXRequirement::Required) {
      inst.opcode.rex_prefix |= REXPrefix::Marker;
    }
    if constexpr ((Index & 8u)!=0u) {
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
