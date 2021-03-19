#include <asm/buffer.hxx>
#include <asm/callable.hxx>
#include <asm/instruction.hxx>

#include <cassert>
#include <cstdint>

#include <iomanip>
#include <ostream>

namespace assembly {
  void Buffer::append(const Instruction& inst)
  {
    append_legacy_prefix(inst);
    append_opcode(inst);
    append_modrm(inst);
    append_immediate(inst);
  }

  Callable Buffer::to_callable() const
  {
    return Callable{data(), size()};
  }

  void Buffer::append_legacy_prefix(Instruction const& inst)
  {
    if (inst.legacy_prefixes.prefix1!=LegacyPrefix1::None) {
      push_back(static_cast<std::byte>(inst.legacy_prefixes.prefix1));
    }
    if (inst.legacy_prefixes.prefix2!=LegacyPrefix2::None) {
      push_back(static_cast<std::byte>(inst.legacy_prefixes.prefix2));
    }
    if (inst.legacy_prefixes.prefix3!=LegacyPrefix3::None) {
      push_back(static_cast<std::byte>(inst.legacy_prefixes.prefix3));
    }
    if (inst.legacy_prefixes.prefix4!=LegacyPrefix4::None) {
      push_back(static_cast<std::byte>(inst.legacy_prefixes.prefix4));
    }
  }

  void Buffer::append_opcode(Instruction const& inst)
  {
    if (inst.opcode.mandatory_prefix!=MandatoryPrefix::None) {
      push_back(static_cast<std::byte>(inst.opcode.mandatory_prefix));
    }
    if (inst.opcode.rex_prefix!=REXPrefix::None) {
      push_back(static_cast<std::byte>(inst.opcode.rex_prefix | REXPrefix::Marker));
    }
    if (inst.opcode.opcode_size!=0) {
      assert(inst.opcode.opcode_size>0 && inst.opcode.opcode_size<4);
      for (std::size_t n = 0u; n<inst.opcode.opcode_size; ++n) {
        push_back(inst.opcode.opcode[n]);
      }
    }
  }

  void Buffer::append_modrm(Instruction const& inst)
  {
    if (inst.mod_rm.value!=0u) {
      push_back(static_cast<std::byte>(inst.mod_rm.value));
    }
  }

  void Buffer::append_immediate(Instruction const& inst)
  {
    std::size_t count;
    switch (inst.immediate.type) {
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
      push_back(inst.immediate.bytes[n]);
    }
  }

  void Buffer::dump(std::ostream& out) const
  {
    out << "Buffer {";
    for (auto const b : *this) {
      out << " 0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(b);
    }
    out << " }" << std::endl;
  }
}
