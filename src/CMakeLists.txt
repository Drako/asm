add_library(asm INTERFACE)
target_sources(asm INTERFACE
    include/asm/register.hxx)
target_include_directories(asm INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
