#include "jit_compiler.hxx"
#include "io.hxx"

#include <asm/instructions/inc_dec.hxx>
#include <asm/instructions/add_sub.hxx>
#include <asm/instructions/mov.hxx>
#include <asm/instructions/jmp.hxx>
#include <asm/instructions/call.hxx>
#include <asm/instructions/test.hxx>

#include <iterator>
#include <numeric>

namespace i = assembly::instructions;
namespace r = assembly::registers;

namespace bf {
  constexpr static auto const pointer = r::AX{}; // used when modifying pointer
  constexpr static auto const pointer64 = r::RAX{}; // used when accessing value through pointer

  constexpr static auto const pointer_backup = r::BX{}; // used when calling functions to save AX
  constexpr static auto const pointer64_backup = r::RBX{};

  constexpr static auto const memory = assembly::addr(r::RSP{}, pointer64, assembly::IndexScale::Scale4);
  constexpr static auto const memory_backup = assembly::addr(r::RSP{}, pointer64_backup, assembly::IndexScale::Scale4);

#ifdef _WIN32
  constexpr static auto const output = r::ECX{};
#else
  constexpr static auto const output = r::EDI{};
#endif
  constexpr static auto const input = r::EAX{};

  std::vector<assembly::Instruction> JitCompiler::operator()(ModifyPointer const& inst) const
  {
    std::vector<assembly::Instruction> result;

    switch (inst.delta) {
    case 0:
      break;
    case 1:
      result.push_back(i::inc(pointer));
      break;
    case -1:
      result.push_back(i::dec(pointer));
      break;
    default:
      result.push_back(i::add_a(pointer, static_cast<std::uint16_t>(inst.delta%65536u)));
    }

    return result;
  }

  std::vector<assembly::Instruction> JitCompiler::operator()(ModifyValue const& inst) const
  {
    std::vector<assembly::Instruction> result;

    switch (inst.delta) {
    case 0:
      break;
    case 1:
      result.push_back(i::inc<std::uint32_t>(memory));
      break;
    case -1:
      result.push_back(i::dec<std::uint32_t>(memory));
      break;
    default:
      result.push_back(i::add(memory, static_cast<std::uint32_t>(inst.delta)));
    }

    return result;
  }

  std::vector<assembly::Instruction> JitCompiler::operator()(PrintValue const&) const
  {
    return {
        i::mov(pointer_backup, pointer),
        i::mov(output, memory),
        i::mov(pointer64, reinterpret_cast<std::uint64_t>(&io::print_char)),
        i::call(pointer64),
        i::mov(pointer, pointer_backup),
    };
  }

  std::vector<assembly::Instruction> JitCompiler::operator()(ScanValue const&) const
  {
    return {
        i::mov(pointer_backup, pointer),
        i::mov(pointer64, reinterpret_cast<std::uint64_t>(&io::scan_char)),
        i::call(pointer64),
        i::mov(memory_backup, input),
        i::mov(pointer, pointer_backup),
    };
  }

  std::vector<assembly::Instruction> JitCompiler::operator()(Scope const& scope) const
  {
    std::vector<assembly::Instruction> result;

    for (auto const& inst : scope.instructions) {
      auto const assembled = std::visit(*this, inst);
      result.reserve(result.size()+assembled.size());
      for (auto const& asm_inst : assembled) {
        result.push_back(asm_inst);
      }
    }

    return result;
  }

  std::vector<assembly::Instruction> JitCompiler::operator()(Loop const& loop) const
  {
    std::vector<assembly::Instruction> result;

    auto const zero_check = i::test<std::uint32_t>(memory, 0xFFFFFFFF);

    std::vector<assembly::Instruction> const body = operator()(loop.scope);
    auto const size = std::accumulate(std::cbegin(body), std::cend(body), 0u,
        [](std::size_t acc, assembly::Instruction const& inst) {
          return acc+inst.size();
        });

    result.push_back(zero_check);
    std::int32_t const jump_distance = size+zero_check.size()+(size>118 ? 6 : 2);

    if (jump_distance>127) {
      result.push_back(i::jz(jump_distance));

      result.reserve(result.size()+body.size());
      for (auto const& inst : body) {
        result.push_back(inst);
      }

      result.push_back(zero_check);
      result.push_back(i::jnz(-jump_distance));
    }
    else {
      result.push_back(i::jz(static_cast<std::int8_t>(jump_distance)));

      result.reserve(result.size()+body.size());
      for (auto const& inst : body) {
        result.push_back(inst);
      }

      result.push_back(zero_check);
      result.push_back(i::jnz(static_cast<std::int8_t>(-jump_distance)));
    }

    return result;
  }
}
