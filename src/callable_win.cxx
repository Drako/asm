#include "asm/callable.hxx"

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <cstring>

namespace assembly {
  Callable::Callable(const std::byte* data, std::size_t size)
      :memory{static_cast<std::byte*>(VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE))},
       size{size}
  {
    // TODO: handle allocation failure
    std::memcpy(memory, data, size);

    DWORD dummy{};
    VirtualProtect(memory, size, PAGE_EXECUTE_READ, &dummy); // TODO: handle error
  }

  Callable::~Callable()
  {
    VirtualFree(memory, 0, MEM_RELEASE);
  }
}
