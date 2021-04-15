#include "executor.hxx"
#include "jit_compiler.hxx"

int main()
{
  // equivalent to BF code:
  // +[,.----------]
  bf::Instruction const echo_program = bf::Scope{{
                                                     bf::ModifyValue{1},
                                                     bf::Loop{{{
                                                                   bf::ScanValue{},
                                                                   bf::PrintValue{},
                                                                   bf::ModifyValue{-'\n'},
                                                               }}},
                                                 }};

#if 0
  bf::VM vm{};
  bf::Executor executor{vm};

  std::visit(executor, echo_program);
#else
  bf::JitCompiler compiler;

  auto const instructions = std::visit(compiler, echo_program);
  auto const compiled = bf::JitCompiler::compile(instructions);
  compiled.call<void>();
#endif

  return 0;
}
