add_executable(bf main.cxx
    instruction.hxx
    executor.cxx executor.hxx
    jit_compiler.cxx jit_compiler.hxx
    io.cxx io.hxx)
target_link_libraries(bf PRIVATE asm)
