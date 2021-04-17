#include "executor.hxx"
#include "jit_compiler.hxx"
#include "lexer.hxx"
#include "parser.hxx"

#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

using namespace std::literals::string_literals;

namespace {
  std::optional<std::vector<char>> read_file(char const* filename)
  {
    std::ifstream in{filename};
    if (!in.is_open()) {
      return {};
    }

    std::vector<char> const result{std::istream_iterator<char>{in}, std::istream_iterator<char>{}};
    return result;
  }
}

int main(int argc, char** argv)
{
  std::vector<char const*> args{argv+1, argv+argc};
  if (args.empty()) {
    std::cout
        << "Usage: \"" << *argv << "\" <options> [filenames...]\n"
        << "Available options:\n"
        << "  --jit      Use the embedded JIT compiler\n"
        << std::endl;
    return 0;
  }

  bool use_jit = false;
  for (char const* arg: args) {
    if ("--jit"s==arg) {
      use_jit = true;
      continue;
    }

    if (auto const opt_source = read_file(arg); opt_source.has_value()) {
      auto const& source = opt_source.value();

      auto const tokens = bf::Lexer::tokenize(source, arg);

      try {
        auto const program = bf::Parser::parse(tokens);

        if (use_jit) {
          bf::VM vm{};
          bf::Executor executor{vm};

          std::visit(executor, program);
        }
        else {
          bf::JitCompiler compiler;

          auto const instructions = std::visit(compiler, program);
          auto const compiled = bf::JitCompiler::compile(instructions);
          compiled.call<void>();
        }
      }
      catch (bf::UnexpectedLoopEnd const& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return -2;
      }
      catch (bf::UnexpectedEndOfFile const& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return -3;
      }
    }
    else {
      std::cerr << "Error: Could not read source file: " << arg << std::endl;
      return -1;
    }
  }

  return 0;
}
