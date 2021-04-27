add_executable(asm_tests
    register_tests.cxx
    )
target_link_libraries(asm_tests PRIVATE asm Catch2::Catch2WithMain)
