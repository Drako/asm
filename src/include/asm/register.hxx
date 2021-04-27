#pragma once

#include <cstdint>

namespace assembly::registers {
  enum class REXRequirement {
    DontCare,
    Forbidden,
    Required,
  };

  enum class GP8 : std::uint8_t {
    AL = 0x0, CL = 0x1, DL = 0x2, BL = 0x3,
    AH = 0x14, CH = 0x15, DH = 0x16, BH = 0x17,
    SPL = 0x4, BPL = 0x5, SIL = 0x6, DIL = 0x7,
    R8L = 0x8, R9L = 0x9, R10L = 0xA, R11L = 0xB,
    R12L = 0xC, R13L = 0xD, R14L = 0xE, R15L = 0xF,
  };

  constexpr std::uint8_t register_id(GP8 r8)
  {
    return static_cast<std::uint8_t>(r8) & 0xF;
  }

  constexpr REXRequirement rex_requirement(GP8 r8)
  {
    switch (r8) {
    case GP8::AH:
    case GP8::CH:
    case GP8::DH:
    case GP8::BH:
      return REXRequirement::Forbidden;
    case GP8::SPL:
    case GP8::BPL:
    case GP8::SIL:
    case GP8::DIL:
      return REXRequirement::Required;
    default:
      return REXRequirement::DontCare;
    }
  }

  enum class GP16 : std::uint8_t {
    AX = 0x0, CX = 0x1, DX = 0x2, BX = 0x3,
    SP = 0x4, BP = 0x5, SI = 0x6, DI = 0x7,
    R8W = 0x8, R9W = 0x9, R10W = 0xA, R11W = 0xB,
    R12W = 0xC, R13W = 0xD, R14W = 0xE, R15W = 0xF,
  };

  constexpr std::uint8_t register_id(GP16 r16)
  {
    return static_cast<std::uint8_t>(r16);
  }

  enum class GP32 : std::uint8_t {
    EAX = 0x0, ECX = 0x1, EDX = 0x2, EBX = 0x3,
    ESP = 0x4, EBP = 0x5, ESI = 0x6, EDI = 0x7,
    R8D = 0x8, R9D = 0x9, R10D = 0xA, R11D = 0xB,
    R12D = 0xC, R13D = 0xD, R14D = 0xE, R15D = 0xF,
  };

  constexpr std::uint8_t register_id(GP32 r32)
  {
    return static_cast<std::uint8_t>(r32);
  }

  enum class GP64 : std::uint8_t {
    RAX = 0x0, RCX = 0x1, RDX = 0x2, RBX = 0x3,
    RSP = 0x4, RBP = 0x5, RSI = 0x6, RDI = 0x7,
    R8 = 0x8, R9 = 0x9, R10 = 0xA, R11 = 0xB,
    R12 = 0xC, R13 = 0xD, R14 = 0xE, R15 = 0xF,
  };

  constexpr std::uint8_t register_id(GP64 r64)
  {
    return static_cast<std::uint8_t>(r64);
  }
}
