#pragma once

#include <cstddef>

#include <iosfwd>
#include <vector>

namespace assembly {
  class Callable;

  struct Instruction;

  class Buffer final : private std::vector<std::byte> {
  public:
    void append(Instruction const& inst);

    [[nodiscard]] Callable to_callable() const;

    void dump(std::ostream& out) const;

  private:
    void append_legacy_prefix(Instruction const& inst);

    void append_opcode(Instruction const& inst);

    void append_modrm(Instruction const& inst);

    void append_sib(Instruction const& inst);

    void append_displacement(Instruction const& inst);

    void append_immediate(Instruction const& inst);
  };
}
