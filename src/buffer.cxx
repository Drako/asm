#include <asm/buffer.hxx>
#include <asm/callable.hxx>
#include <asm/instruction.hxx>

#include <cassert>

#include <iomanip>
#include <sstream>

namespace assembly {
  void Buffer::append(const Instruction& inst)
  {
    for (auto const b : inst.encode()) {
      bytes.push_back(b);
    }
  }

  Callable Buffer::to_callable() const
  {
    return Callable{bytes};
  }

  void Buffer::dump(std::ostream& out) const
  {
    out << "\"";
    for (auto const b : bytes) {
      out << "\\x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(b);
    }
    out << "\"" << std::endl;
  }

  void Buffer::append_zeroes(std::uint16_t count)
  {
    for (; count--;) {
      bytes.push_back(static_cast<std::byte>(0u));
    }
  }

  void Buffer::append_nops(std::uint16_t count)
  {
    for (; count--;) {
      bytes.push_back(static_cast<std::byte>(0x90));
    }
  }

  void Buffer::append_string(char const* str, bool including_null)
  {
    while (*str) {
      bytes.push_back(*reinterpret_cast<std::byte const*>(str));
      ++str;
    }
    if (including_null) {
      bytes.push_back(static_cast<std::byte>(0u));
    }
  }

  std::string Buffer::to_string() const
  {
    std::ostringstream builder;
    dump(builder);
    return builder.str();
  }
}
