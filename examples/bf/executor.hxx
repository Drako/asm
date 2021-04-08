#pragma once

#include "instruction.hxx"

#include <array>

#include <cstdint>

namespace bf {
  struct VM {
    std::uint16_t pointer = 0u;
    std::array<int, 65336u> memory{};
  };

  struct Executor final {
    explicit Executor(VM& state) noexcept;

    void operator()(ModifyPointer const& inst);

    void operator()(ModifyValue const& inst);

    void operator()(PrintValue const& inst);

    void operator()(ScanValue const& inst);

    void operator()(Scope const& scope);

    void operator()(Loop const& loop);

  private:
    VM state;
  };
}
