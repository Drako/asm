#include <asm/buffer.hxx>
#include <asm/callable.hxx>
#include <asm/instruction.hxx>

#include <cassert>

#include <iomanip>
#include <ostream>

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
    out << "Buffer { \"";
    for (auto const b : bytes) {
      out << "\\x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(b);
    }
    out << "\" }" << std::endl;
  }
}
