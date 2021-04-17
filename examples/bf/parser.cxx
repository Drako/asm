#include "parser.hxx"

using namespace std::literals::string_literals;

namespace bf {
  UnexpectedLoopEnd::UnexpectedLoopEnd(std::string const& message)
      :runtime_error(message)
  {
  }

  UnexpectedEndOfFile::UnexpectedEndOfFile(std::string const& message)
      :std::runtime_error(message)
  {
  }

  Instruction Parser::parse(std::vector<TokenInfo> const& tokens)
  {
    auto current = tokens.begin();
    auto const end = tokens.end();

    Instruction program = parse_scope(current, end);
    if (current!=end) {
      throw UnexpectedLoopEnd{
          "Unexpected loop end ']' at "s+current->filename+":"+std::to_string(current->line)+":"
              +std::to_string(current->column)};
    }
    return program;
  }

  Scope Parser::parse_scope(token_iterator& current, token_iterator end)
  {
    std::vector<Instruction> instructions;

    for (; current!=end; ++current) {
      switch (current->tok) {
      case Token::IncrementValue:
        if (!instructions.empty() && std::holds_alternative<ModifyValue>(instructions.back())) {
          auto& last = std::get<ModifyValue>(instructions.back());
          ++last.delta;
          if (!last.delta) instructions.pop_back();
        }
        else {
          instructions.emplace_back(ModifyValue{1});
        }
        break;
      case Token::DecrementValue:
        if (!instructions.empty() && std::holds_alternative<ModifyValue>(instructions.back())) {
          auto& last = std::get<ModifyValue>(instructions.back());
          --last.delta;
          if (!last.delta) instructions.pop_back();
        }
        else {
          instructions.emplace_back(ModifyValue{-1});
        }
        break;
      case Token::IncrementPointer:
        if (!instructions.empty() && std::holds_alternative<ModifyPointer>(instructions.back())) {
          auto& last = std::get<ModifyPointer>(instructions.back());
          ++last.delta;
          if (!last.delta) instructions.pop_back();
        }
        else {
          instructions.emplace_back(ModifyPointer{1});
        }
        break;
      case Token::DecrementPointer:
        if (!instructions.empty() && std::holds_alternative<ModifyPointer>(instructions.back())) {
          auto& last = std::get<ModifyPointer>(instructions.back());
          --last.delta;
          if (!last.delta) instructions.pop_back();
        }
        else {
          instructions.emplace_back(ModifyPointer{-1});
        }
        break;
      case Token::PrintValue:
        instructions.emplace_back(PrintValue{});
        break;
      case Token::ScanValue:
        instructions.emplace_back(ScanValue{});
        break;
      case Token::EndLoop:
        return Scope{instructions};
      case Token::BeginLoop:
        auto const start = current;
        auto loop = Loop{parse_scope(++current, end)};
        if (current==end) {
          throw UnexpectedEndOfFile{
              "Unexpected end of file. Loop '[' started at "s+start->filename+":"+std::to_string(start->line)+":"
                  +std::to_string(start->column)};
        };
        instructions.emplace_back();
        break;
      }
    }

    return Scope{instructions};
  }
}
