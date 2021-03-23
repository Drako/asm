#include <catch2/catch.hpp>

#include <asm/buffer.hxx>
#include <asm/callable.hxx>
#include <asm/instructions/call.hxx>
#include <asm/instructions/jmp.hxx>
#include <asm/instructions/lea.hxx>
#include <asm/instructions/mov.hxx>
#include <asm/instructions/ret.hxx>

#include <sstream>

namespace r = assembly::registers;
namespace i = assembly::instructions;

TEST_CASE("Assembling and running", "[buffer][callable][instruction]")
{
  SECTION("can generate code for function returning int and then run it")
  {
    // []() -> int { return 42; }
    assembly::Buffer memory{};
    memory.append(i::mov(r::EAX{}, 42u));
    memory.append(i::retn());

    std::ostringstream bytes;
    memory.dump(bytes);
    INFO("Generated code: " << bytes.str());

    auto const answer = memory.to_callable();

    auto const result = answer.call<int>();
    REQUIRE(result==42);
  }

  SECTION("can generate code for identity function and then run it")
  {
    // [](char a) -> char { return a; }
    assembly::Buffer memory{};
#ifdef _WIN32
    memory.append(i::mov(r::AL{}, r::CL{}));
#else
    memory.append(i::mov(r::AL{}, r::DIL{}));
#endif
    memory.append(i::retn());

    std::ostringstream bytes;
    memory.dump(bytes);
    INFO("Generated code: " << bytes.str());

    auto const identity = memory.to_callable();

    auto const result = identity.call<char>('A');
    REQUIRE(result=='A');
  }

  SECTION("can generate code for add function and then run it")
  {
    // [](int a, int b) -> int { return a + b; }
    assembly::Buffer memory{};
#ifdef _WIN32
    memory.append(i::lea(r::EAX{}, assembly::addr(r::ECX{}, r::EDX{})));
#else
    memory.append(i::lea(r::EAX{}, assembly::addr(r::EDI{}, r::ESI{})));
#endif
    memory.append(i::retn());

    std::ostringstream bytes;
    memory.dump(bytes);
    INFO("Generated code: " << bytes.str());

    auto const add = memory.to_callable();

    auto const result = add.call<int>(19, 23);
    REQUIRE(result==42);
  }

  SECTION("can generate code for function calling callback and then run it")
  {
    int (* multiply)(int, int) = [](int a, int b) { return a*b; };

    // [](int a, int b, int(*callback)(int, int)) -> int { return callback(a, b); }
    assembly::Buffer memory{};
#ifdef _WIN32
    memory.append(i::jmp(r::R8{}));
#else
    memory.append(i::jmp(r::RDX{}));
#endif

    std::ostringstream bytes;
    memory.dump(bytes);
    INFO("Generated code: " << bytes.str());

    auto const apply = memory.to_callable();

    auto const result = apply.call<int>(6, 7, multiply);
    REQUIRE(result==42);
  }

  SECTION("can generate code for function calling function and then run it")
  {
    // [](char const *str) -> int { return std::atoi(str); }
    assembly::Buffer memory{};
    auto const atoi_symbol = memory.set_symbol("atoi");
    memory.append_bytes(&std::atoi);

    auto const fn_symbol = memory.set_symbol("fn");
    memory.append(i::jmp_rip(atoi_symbol-memory.here()));

    std::ostringstream bytes;
    memory.dump(bytes);
    INFO("Generated code: " << bytes.str());

    auto const fn = memory.to_callable();

    auto const result = fn.call_addr<int>(fn_symbol, "23");
    REQUIRE(result==23);
  }
}
