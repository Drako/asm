#include <asm/instruction.hxx>

#include <cassert>

namespace assembly {
  std::vector<std::byte> Instruction::encode() const
  {
    constexpr std::size_t const MAX_INSTRUCTION_SIZE = 15u;

    std::vector<std::byte> bytes;
    bytes.reserve(MAX_INSTRUCTION_SIZE);

    append_legacy_prefix(bytes);
    append_opcode(bytes);
    append_modrm(bytes);
    append_sib(bytes);
    append_displacement(bytes);
    append_immediate(bytes);

    assert(bytes.size()<=MAX_INSTRUCTION_SIZE);
    return bytes;
  }

  std::size_t Instruction::size() const
  {
    return encode().size();
  }

  void Instruction::append_legacy_prefix(std::vector<std::byte>& bytes) const
  {
    if (legacy_prefixes.prefix1!=LegacyPrefix1::None) {
      bytes.push_back(static_cast<std::byte>(legacy_prefixes.prefix1));
    }
    if (legacy_prefixes.prefix2!=LegacyPrefix2::None) {
      bytes.push_back(static_cast<std::byte>(legacy_prefixes.prefix2));
    }
    if (legacy_prefixes.prefix3!=LegacyPrefix3::None) {
      bytes.push_back(static_cast<std::byte>(legacy_prefixes.prefix3));
    }
    if (legacy_prefixes.prefix4!=LegacyPrefix4::None) {
      bytes.push_back(static_cast<std::byte>(legacy_prefixes.prefix4));
    }
  }

  void Instruction::append_opcode(std::vector<std::byte>& bytes) const
  {
    if (opcode.mandatory_prefix!=MandatoryPrefix::None) {
      bytes.push_back(static_cast<std::byte>(opcode.mandatory_prefix));
    }
    if (opcode.rex_prefix!=REXPrefix::None) {
      bytes.push_back(static_cast<std::byte>(opcode.rex_prefix | REXPrefix::Marker));
    }
    if (opcode.opcode_size!=0) {
      assert(opcode.opcode_size>0 && opcode.opcode_size<4);
      for (std::size_t n = 0u; n<opcode.opcode_size; ++n) {
        bytes.push_back(opcode.opcode[n]);
      }
    }
  }

  void Instruction::append_modrm(std::vector<std::byte>& bytes) const
  {
    if (mod_rm.value!=0u) {
      bytes.push_back(static_cast<std::byte>(mod_rm.value));
    }
  }

  void Instruction::append_sib(std::vector<std::byte>& bytes) const
  {
    if (sib.has_value()) {
      bytes.push_back(static_cast<std::byte>(sib.value().value));
    }
  }

  void Instruction::append_displacement(std::vector<std::byte>& bytes) const
  {
    if (!displacement.has_value())
      return;

    auto const& disp = displacement.value();

    std::size_t count;
    switch (disp.type) {
    case DisplacementType::Disp8:
      count = 1u;
      break;
    case DisplacementType::Disp16:
      count = 2u;
      break;
    case DisplacementType::Disp32:
      count = 4u;
      break;
    case DisplacementType::Disp64:
      count = 8u;
      break;
    default:
      count = 0u;
    }

    for (std::size_t n = 0u; n<count; ++n) {
      bytes.push_back(disp.bytes[n]);
    }
  }

  void Instruction::append_immediate(std::vector<std::byte>& bytes) const
  {
    if (!immediate.has_value())
      return;

    auto const& imm = immediate.value();

    std::size_t count;
    switch (imm.type) {
    case ImmediateType::Imm8:
      count = 1u;
      break;
    case ImmediateType::Imm16:
      count = 2u;
      break;
    case ImmediateType::Imm32:
      count = 4u;
      break;
    case ImmediateType::Imm64:
      count = 8u;
      break;
    default:
      count = 0u;
    }

    for (std::size_t n = 0u; n<count; ++n) {
      bytes.push_back(imm.bytes[n]);
    }
  }
}
