#include <catch2/catch.hpp>

#include <asm/callable.hxx>

TEST_CASE("Callable", "[callable]")
{
  SECTION("can call function returning a number")
  {
    // []() -> int { return 42; }
    char const code[] = "\xB8\x2A\0\0\0\xC3";
    assembly::Callable answer{reinterpret_cast<std::byte const*>(code), sizeof(code)};

    auto const result = answer.call<int>();
    REQUIRE(result==42);
  }

  SECTION("can call function with parameters")
  {
    // [](int a, int b) -> int { return a + b; }
#ifdef _WIN32
    // on windows parameters are passed via ecx and edx
    char const code[] = "\x67\x8D\x04\x11\xC3";
#else
    // on unix parameters are passed via edi and esi
    char const code[] = "\x67\x8D\x04\x37\xC3";
#endif
    assembly::Callable add{reinterpret_cast<std::byte const*>(code), sizeof(code)};

    auto const result = add.call<int>(19, 23);
    REQUIRE(result==42);
  }
}
