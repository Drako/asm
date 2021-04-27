#include <catch2/catch.hpp>

#include <asm/register.hxx>

#include <cassert>
#include <array>

namespace r = assembly::registers;

TEST_CASE("Registers")
{
  SECTION("8 Bit General Purpose") {
    auto const registers = std::array{
        r::GP8::AL, r::GP8::CL, r::GP8::DL, r::GP8::BL,
        r::GP8::AH, r::GP8::CH, r::GP8::DH, r::GP8::BH,
        r::GP8::SPL, r::GP8::BPL, r::GP8::SIL, r::GP8::DIL,
        r::GP8::R8L, r::GP8::R9L, r::GP8::R10L, r::GP8::R11L,
        r::GP8::R12L, r::GP8::R13L, r::GP8::R14L, r::GP8::R15L,
    };

    SECTION("Register ID") {
      auto const ids = std::array<std::uint8_t, 20u>{
          0, 1, 2, 3,
          4, 5, 6, 7,
          4, 5, 6, 7,
          8, 9, 10, 11,
          12, 13, 14, 15,
      };
      assert(registers.size()==ids.size());

      for (auto n = 0u; n<ids.size(); ++n) {
        INFO("index: " << n)
        REQUIRE(r::register_id(registers[n])==ids[n]);
      }
    }

    SECTION("REX requirement") {
      auto const reqs = std::array{
          r::REXRequirement::DontCare, r::REXRequirement::DontCare,
          r::REXRequirement::DontCare, r::REXRequirement::DontCare,
          r::REXRequirement::Forbidden, r::REXRequirement::Forbidden,
          r::REXRequirement::Forbidden, r::REXRequirement::Forbidden,
          r::REXRequirement::Required, r::REXRequirement::Required,
          r::REXRequirement::Required, r::REXRequirement::Required,
          r::REXRequirement::DontCare, r::REXRequirement::DontCare,
          r::REXRequirement::DontCare, r::REXRequirement::DontCare,
          r::REXRequirement::DontCare, r::REXRequirement::DontCare,
          r::REXRequirement::DontCare, r::REXRequirement::DontCare,
      };
      assert(registers.size()==reqs.size());

      for (auto n = 0u; n<reqs.size(); ++n) {
        INFO("index: " << n)
        REQUIRE(r::rex_requirement(registers[n])==reqs[n]);
      }
    }
  }

  SECTION("16 Bit General Purpose")
  {
    auto const registers = std::array{
        r::GP16::AX, r::GP16::CX, r::GP16::DX, r::GP16::BX,
        r::GP16::SP, r::GP16::BP, r::GP16::SI, r::GP16::DI,
        r::GP16::R8W, r::GP16::R9W, r::GP16::R10W, r::GP16::R11W,
        r::GP16::R12W, r::GP16::R13W, r::GP16::R14W, r::GP16::R15W,
    };

    SECTION("Register ID") {
      auto const ids = std::array<std::uint8_t, 16u>{
          0, 1, 2, 3,
          4, 5, 6, 7,
          8, 9, 10, 11,
          12, 13, 14, 15,
      };
      assert(registers.size()==ids.size());

      for (auto n = 0u; n<ids.size(); ++n) {
        INFO("index: " << n)
        REQUIRE(r::register_id(registers[n])==ids[n]);
      }
    }
  }

  SECTION("32 Bit General Purpose")
  {
    auto const registers = std::array{
        r::GP32::EAX, r::GP32::ECX, r::GP32::EDX, r::GP32::EBX,
        r::GP32::ESP, r::GP32::EBP, r::GP32::ESI, r::GP32::EDI,
        r::GP32::R8D, r::GP32::R9D, r::GP32::R10D, r::GP32::R11D,
        r::GP32::R12D, r::GP32::R13D, r::GP32::R14D, r::GP32::R15D,
    };

    SECTION("Register ID") {
      auto const ids = std::array<std::uint8_t, 16u>{
          0, 1, 2, 3,
          4, 5, 6, 7,
          8, 9, 10, 11,
          12, 13, 14, 15,
      };
      assert(registers.size()==ids.size());

      for (auto n = 0u; n<ids.size(); ++n) {
        INFO("index: " << n)
        REQUIRE(r::register_id(registers[n])==ids[n]);
      }
    }
  }

  SECTION("64 Bit General Purpose")
  {
    auto const registers = std::array{
        r::GP64::RAX, r::GP64::RCX, r::GP64::RDX, r::GP64::RBX,
        r::GP64::RSP, r::GP64::RBP, r::GP64::RSI, r::GP64::RDI,
        r::GP64::R8, r::GP64::R9, r::GP64::R10, r::GP64::R11,
        r::GP64::R12, r::GP64::R13, r::GP64::R14, r::GP64::R15,
    };

    SECTION("Register ID") {
      auto const ids = std::array<std::uint8_t, 16u>{
          0, 1, 2, 3,
          4, 5, 6, 7,
          8, 9, 10, 11,
          12, 13, 14, 15,
      };
      assert(registers.size()==ids.size());

      for (auto n = 0u; n<ids.size(); ++n) {
        INFO("index: " << n)
        REQUIRE(r::register_id(registers[n])==ids[n]);
      }
    }
  }
}
