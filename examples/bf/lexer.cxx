#include "lexer.hxx"

namespace bf {
  std::vector<TokenInfo> Lexer::tokenize(std::vector<char> const& src, char const* filename)
  {
    std::vector<TokenInfo> tokens;
    TokenInfo info{filename};

    for (auto const c : src) {
      switch (c) {
      default:
        break;
      case '\n':
        ++info.line;
        info.column = 0u;
        continue;
      case '+':
        info.tok = Token::IncrementValue;
        tokens.push_back(info);
        break;
      case '-':
        info.tok = Token::DecrementValue;
        tokens.push_back(info);
        break;
      case '>':
        info.tok = Token::IncrementPointer;
        tokens.push_back(info);
        break;
      case '<':
        info.tok = Token::DecrementPointer;
        tokens.push_back(info);
        break;
      case '.':
        info.tok = Token::PrintValue;
        tokens.push_back(info);
        break;
      case ',':
        info.tok = Token::ScanValue;
        tokens.push_back(info);
        break;
      case '[':
        info.tok = Token::BeginLoop;
        tokens.push_back(info);
        break;
      case ']':
        info.tok = Token::EndLoop;
        tokens.push_back(info);
        break;
      }

      ++info.column;
    }

    return tokens;
  }
}
