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

    template<typename Container>
    inline void append(Container const& instances)
    {
      for (auto const& inst : instances) {
        append(inst);
      }
    }

    template<typename T>
    inline void append_bytes(T data)
    {
      detail::AsBytes<T> converter;
      converter.original = data;
      for (auto b : converter.bytes) {
        bytes.push_back(b);
      }
    }

    void append_zeroes(std::uint16_t count);

    void append_nops(std::uint16_t count);

    void append_string(char const* str, bool including_null = true);

    [[nodiscard]] Callable to_callable() const;

    void dump(std::ostream& out) const;

    [[nodiscard]] std::string to_string() const;

  private:
    std::vector<std::byte> bytes{};
  };
}
