#pragma once

#include "instruction.hxx"
#include "token.hxx"

#include <stdexcept>
#include <string>
#include <vector>

namespace bf {
  using token_iterator = std::vector<TokenInfo>::const_iterator;

  struct UnexpectedLoopEnd : std::runtime_error {
    explicit UnexpectedLoopEnd(std::string const& message);
  };

  struct UnexpectedEndOfFile : std::runtime_error {
    explicit UnexpectedEndOfFile(std::string const& message);
  };

  struct Parser {
    static Instruction parse(std::vector<TokenInfo> const& tokens);

    static Scope parse_scope(token_iterator& current, token_iterator end);
  };
}
