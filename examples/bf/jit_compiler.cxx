#include "jit_compiler.hxx"
#include "io.hxx"

#include <asm/instructions/all.hxx>
#include <asm/buffer.hxx>

#include <cstring>
#include <iterator>
#include <numeric>

namespace i = assembly::instructions;
namespace r = assembly::registers;

#ifdef _WIN32
extern "C" void __chkstk();
#endif

namespace bf {
  constexpr static auto const pointer = r::BX{}; // used when modifying pointer
  constexpr static auto const pointer64 = r::RBX{}; // used when accessing value through pointer

  constexpr static auto const memory = assembly::addr(r::RSP{}, pointer64, assembly::IndexScale::Scale4);

#ifdef _WIN32
  constexpr static auto const output = r::ECX{};
  constexpr static auto const dest = r::RCX{};
  constexpr static auto const ch = r::EDX{};
  constexpr static auto const count = r::R8{};
#else
  constexpr static auto const output = r::EDI{};
  constexpr static auto const dest = r::RDI{};
  constexpr static auto const ch = r::ESI{};
  constexpr static auto const count = r::RDX{};
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
      result.push_back(i::add(pointer, static_cast<std::uint16_t>(inst.delta%65536u)));
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
    std::vector<assembly::Instruction> instructions;
    instructions.reserve(5u);

    instructions.push_back(i::mov(output, memory));
    for (auto const& inst : i::call(io::print_char)) {
      instructions.push_back(inst);
    }

    return instructions;
  }

  std::vector<assembly::Instruction> JitCompiler::operator()(ScanValue const&) const
  {
    std::vector<assembly::Instruction> instructions;
    instructions.reserve(5u);

    for (auto const& inst : i::call(io::scan_char)) {
      instructions.push_back(inst);
    }
    instructions.push_back(i::mov(memory, input));

    return instructions;
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
    auto const size = std::accumulate(std::cbegin(body), std::cend(body), static_cast<std::size_t>(0u),
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

  assembly::Callable JitCompiler::compile(std::vector<assembly::Instruction> const& instructions)
  {
    assembly::Buffer buffer;

    constexpr static std::uint32_t const MEM_SIZE = 65536u << 2u;

    buffer.append(i::push(pointer64));
#ifdef _WIN32
    buffer.append(i::mov(pointer64, reinterpret_cast<std::uint64_t>(&__chkstk)));
    buffer.append(i::mov(r::RAX{}, MEM_SIZE+64ull));
    buffer.append(i::call(pointer64));
#endif
    buffer.append(i::sub64(r::RSP{}, MEM_SIZE));

    // make sure pointer is initialized with 0
    buffer.append(i::xor_(pointer64, pointer64));

    // make sure memory is filled with zeroes
    buffer.append(i::mov(dest, r::RSP{}));
    buffer.append(i::xor_(ch, ch));
    buffer.append(i::mov(count, static_cast<std::uint64_t>(MEM_SIZE)));
    buffer.append(i::call(std::memset));

    // actual body
    buffer.append(instructions);

    // cleanup
    buffer.append(i::add64(r::RSP{}, MEM_SIZE));
    buffer.append(i::pop(pointer64));
    buffer.append(i::retn());

    return buffer.to_callable();
  }
}
