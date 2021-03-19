#pragma once

#include <cstdint>

namespace assembly {
  template<typename ValueType>
  struct Register final {
    using value_type = ValueType;

    std::uint8_t xreg;
    bool need_rex = false;
  };

  namespace registers {
    static Register<std::uint8_t> const AL{0u};
    static Register<std::uint8_t> const AH{4u};
    static Register<std::uint16_t> const AX{0u};
    static Register<std::uint32_t> const EAX{0u};
    static Register<std::uint64_t> const RAX{0u};

    static Register<std::uint8_t> const BL{3u};
    static Register<std::uint8_t> const BH{7u};
    static Register<std::uint16_t> const BX{3u};
    static Register<std::uint32_t> const EBX{3u};
    static Register<std::uint64_t> const RBX{3u};

    static Register<std::uint8_t> const CL{1u};
    static Register<std::uint8_t> const CH{5u};
    static Register<std::uint16_t> const CX{1u};
    static Register<std::uint32_t> const ECX{1u};
    static Register<std::uint64_t> const RCX{1u};

    static Register<std::uint8_t> const DL{2u};
    static Register<std::uint8_t> const DH{6u};
    static Register<std::uint16_t> const DX{2u};
    static Register<std::uint32_t> const EDX{2u};
    static Register<std::uint64_t> const RDX{2u};

    static Register<std::uint8_t> const SIL{6u, true};
    static Register<std::uint16_t> const SI{6u};
    static Register<std::uint32_t> const ESI{6u};
    static Register<std::uint64_t> const RSI{6u};

    static Register<std::uint8_t> const DIL{7u, true};
    static Register<std::uint16_t> const DI{7u};
    static Register<std::uint32_t> const EDI{7u};
    static Register<std::uint64_t> const RDI{7u};
  }
}
