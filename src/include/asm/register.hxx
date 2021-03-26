#pragma once

#include "types.hxx"

namespace assembly {
  enum class REXRequirement {
    DontCare = 0,
    Required,
    Forbidden,
  };

  template<typename ValueType, std::uint8_t Index, REXRequirement Rex = REXRequirement::DontCare, typename Valid = void>
  struct Register;

  template<typename ValueType, std::uint8_t Index, REXRequirement Rex>
  struct Register<ValueType, Index, Rex, std::enable_if_t<
      types::IsOneOfV<ValueType, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t> && (Index<=15u)
  >> final {
    using value_type = ValueType;
    using signed_type = types::SignedTypeT<ValueType>;
    constexpr static std::uint8_t const index = Index;
    constexpr static REXRequirement const rex = Rex;
  };

  template<std::uint8_t Index>
  struct XMM {
    constexpr static std::uint8_t const index = Index;
  };

  namespace registers {
#define ASM_BASE_REGISTERS(letter, idx) \
    using letter##L = Register<std::uint8_t, idx>; \
    using letter##H = Register<std::uint8_t, idx+4u, REXRequirement::Forbidden>; \
    using letter##X = Register<std::uint16_t, idx>; \
    using E##letter##X = Register<std::uint32_t, idx>; \
    using R##letter##X = Register<std::uint64_t, idx>;
    ASM_BASE_REGISTERS(A, 0u)
    ASM_BASE_REGISTERS(B, 3u)
    ASM_BASE_REGISTERS(C, 1u)
    ASM_BASE_REGISTERS(D, 2u)
#undef ASM_BASE_REGISTERS

#define ASM_POINTER_REGISTERS(base, idx) \
    using base##L = Register<std::uint8_t, idx, REXRequirement::Required>; \
    using base = Register<std::uint16_t, idx>; \
    using E##base = Register<std::uint32_t, idx>; \
    using R##base = Register<std::uint64_t, idx>;
    ASM_POINTER_REGISTERS(SP, 4u)
    ASM_POINTER_REGISTERS(BP, 5u)
    ASM_POINTER_REGISTERS(SI, 6u)
    ASM_POINTER_REGISTERS(DI, 7u)
#undef ASM_POINTER_REGISTERS

    // special case
    // [rbp+disp32] turns into [rip+disp32] by having ModR/M.mod = 0
    using RIP = RBP;

#define ASM_EXTENDED_REGISTERS(idx) \
    using R##idx##L = Register<std::uint8_t, idx##u>; \
    using R##idx##W = Register<std::uint16_t, idx##u>; \
    using R##idx##D = Register<std::uint32_t, idx##u>; \
    using R##idx = Register<std::uint64_t, idx##u>;
    ASM_EXTENDED_REGISTERS(8)
    ASM_EXTENDED_REGISTERS(9)
    ASM_EXTENDED_REGISTERS(10)
    ASM_EXTENDED_REGISTERS(11)
    ASM_EXTENDED_REGISTERS(12)
    ASM_EXTENDED_REGISTERS(13)
    ASM_EXTENDED_REGISTERS(14)
    ASM_EXTENDED_REGISTERS(15)
#undef ASM_EXTENDED_REGISTERS

    using XMM0 = XMM<0u>;
    using XMM1 = XMM<1u>;
    using XMM2 = XMM<2u>;
    using XMM3 = XMM<3u>;
    using XMM4 = XMM<4u>;
    using XMM5 = XMM<5u>;
    using XMM6 = XMM<6u>;
    using XMM7 = XMM<7u>;
    using XMM8 = XMM<8u>;
    using XMM9 = XMM<9u>;
    using XMM10 = XMM<10u>;
    using XMM11 = XMM<11u>;
    using XMM12 = XMM<12u>;
    using XMM13 = XMM<13u>;
    using XMM14 = XMM<14u>;
    using XMM15 = XMM<15u>;
  }
}
