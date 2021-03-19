#include <catch2/catch.hpp>

#include <asm/buffer.hxx>
#include <asm/callable.hxx>
#include <asm/instructions/mov.hxx>
#include <asm/instructions/ret.hxx>

#include <sstream>

TEST_CASE("can generate code for function returning int and then run it", "[buffer][callable][instruction]")
{
  // []() -> int { return 42; }
  assembly::Buffer memory{};
  memory.append(assembly::instructions::mov(assembly::registers::EAX, 42));
  memory.append(assembly::instructions::retn());

  std::ostringstream bytes;
  memory.dump(bytes);
  INFO("Generated code: " << bytes.str());

  auto const answer = memory.to_callable();

  auto const result = answer.call<int>();
  REQUIRE(result == 42);
}

TEST_CASE("can generate code for identity function and then run it", "[buffer][callable][instruction]")
{
  // [](char a) -> char { return a; }
  assembly::Buffer memory{};
#ifdef _WIN32
  memory.append(assembly::instructions::mov(assembly::registers::AL, assembly::registers::CL));
#else
  memory.append(assembly::instructions::mov(assembly::registers::AL, assembly::registers::DIL));
#endif
  memory.append(assembly::instructions::retn());

  std::ostringstream bytes;
  memory.dump(bytes);
  INFO("Generated code: " << bytes.str());

  auto const answer = memory.to_callable();

  auto const result = answer.call<char>('A');
  REQUIRE(result == 'A');
}
