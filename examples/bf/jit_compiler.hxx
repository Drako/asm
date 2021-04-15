#pragma once

#include <asm/callable.hxx>
#include <asm/instruction.hxx>

#include <vector>

#include "instruction.hxx"

namespace bf {
  // ATTENTION: the generated code handles only the function body
  // it does not save used registers (RBX) nor does it allocate the required stack memory (65536 ints)
  struct JitCompiler final {
    std::vector<assembly::Instruction> operator()(ModifyPointer const& inst) const;

    std::vector<assembly::Instruction> operator()(ModifyValue const& inst) const;

    std::vector<assembly::Instruction> operator()(PrintValue const& inst) const;

    std::vector<assembly::Instruction> operator()(ScanValue const& inst) const;

    std::vector<assembly::Instruction> operator()(Scope const& scope) const;

    std::vector<assembly::Instruction> operator()(Loop const& loop) const;

    // this function actually wraps the instructions into an actual callable function
    static assembly::Callable compile(std::vector<assembly::Instruction> const& instructions);
  };
}
