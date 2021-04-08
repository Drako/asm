#pragma once

#include <variant>
#include <vector>

namespace bf {
  struct ModifyPointer final {
    int delta;
  };

  struct ModifyValue final {
    int delta;
  };

  struct PrintValue final {
  };

  struct ScanValue final {
  };

  struct Scope;

  struct Loop;

  using Instruction = std::variant<ModifyPointer, ModifyValue, PrintValue, ScanValue, Scope, Loop>;

  struct Scope final {
    std::vector<Instruction> instructions;
  };

  struct Loop final {
    Scope scope;
  };
}
