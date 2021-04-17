#pragma once

namespace bf {
  enum class Token {
    IncrementValue, // +
    DecrementValue, // -
    IncrementPointer, // >
    DecrementPointer, // <
    PrintValue, // .
    ScanValue, // ,
    BeginLoop, // [
    EndLoop, // ]
  };

  struct TokenInfo {
    char const* filename;
    unsigned line = 1u;
    unsigned column = 0u;
    Token tok;
  };
}
