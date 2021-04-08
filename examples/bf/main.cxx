#include "executor.hxx"

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

  bf::VM vm{};
  bf::Executor executor{vm};

  std::visit(executor, echo_program);
  return 0;
}
