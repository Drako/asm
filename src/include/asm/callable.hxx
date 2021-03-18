#pragma once

#include <cstddef>

#include <array>
#include <type_traits>
#include <vector>

namespace assembly {
  class Callable final {
  public:
    Callable(std::byte const* data, std::size_t size);

    template<std::size_t Size>
    explicit inline Callable(std::array<std::byte, Size> const& data)
        : Callable{data.data(), data.size()} { }

    explicit inline Callable(std::vector<std::byte> const& data)
        :Callable{data.data(), data.size()} { }

    inline Callable(Callable const& src)
        :Callable{src.memory, src.size} { }

    inline Callable(Callable&& src) noexcept
        :memory{src.memory}, size{src.size} { src.memory = nullptr; }

    ~Callable();

    template<typename Return, typename... Args>
    inline Return call(Args ... args) const
    {
      using FunctionPointer = Return(*)(Args...);
      auto const ptr = reinterpret_cast<FunctionPointer>(memory);
      return ptr(args...);
    }

  private:
    std::byte* memory;
    std::size_t size;
  };
}
