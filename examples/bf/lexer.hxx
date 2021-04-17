#pragma once

#include "token.hxx"

#include <vector>

namespace bf {
  struct Lexer {
    static std::vector<TokenInfo> tokenize(std::vector<char> const& src, char const* filename);
  };
}
