set(asm_HEADERS
    include/asm/buffer.hxx
    include/asm/callable.hxx
    include/asm/memory.hxx
    include/asm/register.hxx
    include/asm/types.hxx
    include/asm/instruction.hxx
    include/asm/instruction_helper.hxx
    include/asm/instructions/jmp.hxx
    include/asm/instructions/lea.hxx
    include/asm/instructions/mov.hxx
    include/asm/instructions/ret.hxx
    )
set(asm_SOURCES
    buffer.cxx
    )

if (WIN32)
  list(APPEND asm_SOURCES
      callable_win.cxx
      )
else ()
  list(APPEND asm_SOURCES
      callable_unix.cxx
      )
endif ()

add_library(asm STATIC
    ${asm_SOURCES}
    ${asm_HEADERS}
    )
target_include_directories(asm PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
