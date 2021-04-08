#include "executor.hxx"
#include "io.hxx"

namespace bf {
  Executor::Executor(VM& state) noexcept
      :state{state}
  {
  }

  void Executor::operator()(ModifyPointer const& inst)
  {
    state.pointer += inst.delta;
  }

  void Executor::operator()(ModifyValue const& inst)
  {
    state.memory[state.pointer] += inst.delta;
  }

  void Executor::operator()(PrintValue const&)
  {
    io::print_char(state.memory[state.pointer]);
  }

  void Executor::operator()(ScanValue const&)
  {
    state.memory[state.pointer] = io::scan_char();
  }

  void Executor::operator()(Scope const& scope)
  {
    for (auto const& inst: scope.instructions) {
      std::visit(*this, inst);
    }
  }

  void Executor::operator()(Loop const& loop)
  {
    while (state.memory[state.pointer]!=0) {
      operator()(loop.scope);
    }
  }
}
