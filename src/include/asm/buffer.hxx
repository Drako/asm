#pragma once

#include <cstddef>

#include <iosfwd>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace assembly {
  namespace detail {
    template<typename T>
    union AsBytes final {
      T original;
      std::byte bytes[sizeof(T)];
    };
  }

  class Callable;

  struct Instruction;

  class Buffer final {
  public:
    void append(Instruction const& inst);

    template<typename T>
    inline void append_bytes(T data)
    {
      detail::AsBytes<T> converter;
      converter.original = data;
      for (auto b : converter.bytes) {
        bytes.push_back(b);
      }
    }

    inline void append_zeroes(std::uint16_t count)
    {
      for (; count--;) {
        bytes.push_back(static_cast<std::byte>(0u));
      }
    }

    inline void append_string(char const* str, bool including_null = true)
    {
      while (*str) {
        bytes.push_back(*reinterpret_cast<std::byte const*>(str));
        ++str;
      }
      if (including_null) {
        bytes.push_back(static_cast<std::byte>(0u));
      }
    }

    std::int32_t set_symbol(std::string const& name);

    [[nodiscard]] std::optional<std::int32_t> get_symbol(std::string const& name) const;

    [[nodiscard]] std::int32_t here() const;

    [[nodiscard]] Callable to_callable() const;

    void dump(std::ostream& out) const;

  private:
    std::vector<std::byte> bytes{};
    std::unordered_map<std::string, std::size_t> symbols;

    void append_legacy_prefix(Instruction const& inst);

    void append_opcode(Instruction const& inst);

    void append_modrm(Instruction const& inst);

    void append_sib(Instruction const& inst);

    void append_displacement(Instruction const& inst);

    void append_immediate(Instruction const& inst);
  };
}
