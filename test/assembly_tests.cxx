#include <catch2/catch.hpp>

#include <asm/buffer.hxx>
#include <asm/callable.hxx>
#include <asm/instructions/add_sub.hxx>
#include <asm/instructions/bitwise.hxx>
#include <asm/instructions/call.hxx>
#include <asm/instructions/inc_dec.hxx>
#include <asm/instructions/jmp.hxx>
#include <asm/instructions/lea.hxx>
#include <asm/instructions/mov.hxx>
#include <asm/instructions/ret.hxx>
#include <asm/instructions/test.hxx>

#include <cstdlib>
#include <cstring>

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
    memory.append_bytes(&std::atoi);

    // 14 = 8 (size of the std::atoi pointer) + 6 (size of the jump instruction)
    // because the target address is taken from [RIP+disp32]
    // but RIP at that point is already after the jmp
    memory.append(i::jmp_rip(-14));

    std::ostringstream bytes;
    memory.dump(bytes);
    INFO("Generated code: " << bytes.str());

    auto const fn = memory.to_callable();

    // just skip the pointer (8 bytes) and call our function
    auto const result = fn.call_addr<int>(8, "23");
    REQUIRE(result==23);
  }

  SECTION("hello world")
  {
    std::string const message{"Hello world!"};

    assembly::Buffer memory{};
    memory.append_bytes(&std::strncpy); // relative address: 0

    memory.append_string(message.c_str()); // relative address: 8

    // relative address: 21
#ifdef _WIN32
    memory.append(i::mov(r::R8{}, r::RDX{}));
    memory.append(i::lea_rip(r::RDX{}, -23));
#else
    memory.append(i::mov(r::RDX{}, r::RSI{}));
    memory.append(i::lea_rip(r::RSI{}, -23));
#endif
    memory.append(i::jmp_rip(-37));

    std::ostringstream bytes;
    memory.dump(bytes);
    INFO("Generated code: " << bytes.str());

    auto const hello = memory.to_callable();

    char buffer[16];
    hello.call_addr<void>(21, buffer, sizeof(buffer));
    REQUIRE(buffer==message);
  }

  SECTION("rna transcription") {
    std::string const expected{"CAUACUUGAGUA"};

#ifdef _WIN32
    auto const src = r::RDX{};
    auto const dest = r::RCX{};
#else
    auto const src = r::RSI{};
    auto const dest = r::RDI{};
#endif
    auto const current = r::RAX{};
    auto const current_char = r::AL{};
    auto const index = r::RBX{};
    auto const mapping = r::R8{};

    assembly::Buffer memory{};
    memory.append(i::xor_(current, current)); // offset: 0
    memory.append(i::xor_(index, index)); // offset: 3
    memory.append(i::lea_rip(mapping, 22)); // offset: 6
    // .loop:
    memory.append(i::mov(current_char, assembly::addr(src, index))); // offset: 13
    memory.append(i::mov(current_char, assembly::addr(mapping, current))); // offset: 16
    memory.append(i::mov(assembly::addr(dest, index), current_char)); // offset: 20
    memory.append(i::inc(index)); // offset: 23
    memory.append(i::test(current_char, current_char)); // offset: 26
    memory.append(i::jne(-21)); // jne .loop -- offset: 28
    memory.append(i::retn()); // offset: 34
    memory.append_zeroes(65u); // offset: 35
    memory.append_string("U G   C            A", false); // offset: 100


    std::ostringstream bytes;
    memory.dump(bytes);
    INFO("Generated code: " << bytes.str());

    auto const rna_transcribe = memory.to_callable();

    char const dna[] = "GTATGAACTCAT";
    char rna[sizeof(dna)] = {0};
    // void rna_transcribe(char * dest, char const * src);
    rna_transcribe.call<void>(rna, dna);
    REQUIRE(rna==expected);
  }
}
