add_executable(asm_tests
    assembly_tests.cxx
    buffer_tests.cxx
    callable_tests.cxx
    )
target_link_libraries(asm_tests PRIVATE asm Catch2::Catch2WithMain)
