#include <catch2/catch.hpp>

#include <asm/buffer.hxx>
#include <asm/callable.hxx>
#include <asm/instructions/lea.hxx>
#include <asm/instructions/mov.hxx>
#include <asm/instructions/ret.hxx>

#include <sstream>

namespace r = assembly::registers;
namespace i = assembly::instructions;

TEST_CASE("can generate code for function returning int and then run it", "[buffer][callable][instruction]")
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

TEST_CASE("can generate code for identity function and then run it", "[buffer][callable][instruction]")
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

  auto const answer = memory.to_callable();

  auto const result = answer.call<char>('A');
  REQUIRE(result=='A');
}

TEST_CASE("can generate code for add function and then run it", "[buffer][callable][instruction]")
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

  auto const answer = memory.to_callable();

  auto const result = answer.call<int>(19, 23);
  REQUIRE(result==42);
}
