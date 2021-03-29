#pragma once

#include <cstddef>

#include <iosfwd>
#include <optional>
#include <string>
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

    inline void append_nops(std::uint16_t count)
    {
      for (; count--;) {
        bytes.push_back(static_cast<std::byte>(0x90));
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

    [[nodiscard]] Callable to_callable() const;

    void dump(std::ostream& out) const;

  private:
    std::vector<std::byte> bytes{};
  };
}
