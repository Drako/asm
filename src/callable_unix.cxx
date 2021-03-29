#include <asm/callable.hxx>

#include <sys/mman.h>

#include <cstring>

namespace assembly {
  Callable::Callable(const std::byte* data, std::size_t size)
      :memory{static_cast<std::byte*>(mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0))},
       size{size}
  {
    // TODO: handle allocation failure
    std::memcpy(memory, data, size);

    mprotect(memory, size, PROT_READ | PROT_EXEC); // TODO: handle error
  }

  Callable::~Callable()
  {
    munmap(memory, size);
  }
}
