cmake_minimum_required(VERSION 3.18)

# ---- Cross toolchain: must be set BEFORE project() ----
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)
set(CMAKE_C_COMPILER   riscv64-linux-gnu-gcc)
set(CMAKE_ASM_COMPILER riscv64-linux-gnu-gcc)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(riscv_tests C ASM)

if(NOT CMAKE_C_COMPILER MATCHES "riscv")
  message(FATAL_ERROR
    "Wrong C compiler: ${CMAKE_C_COMPILER}\n"
    "Delete the build directory and reconfigure: rm -rf build")
endif()

set(RV_FLAGS -march=rv32im -mabi=ilp32)
add_compile_options(${RV_FLAGS} -g -ffreestanding)
add_link_options(${RV_FLAGS} -nostdlib -static)

# ---- Tools ----
find_program(QEMU qemu-riscv32 REQUIRED)
find_program(GDB  gdb-multiarch)          # optional, only for debug targets

enable_testing()

# ---- Inter-function dependencies (optional) ----
# Syntax: set(<fn>_DEPS <other fns it calls>)
# Example: max calls sub
# set(max_DEPS sub)

# ---- Test runtime: provides _start (object library, linked into every test) ----
add_library(start OBJECT tests/common/start.s)

# ---- Helper script for debug targets ----
file(WRITE ${CMAKE_BINARY_DIR}/debug.sh [=[
#!/bin/sh
# usage: debug.sh <qemu> <gdb> <exe>
QEMU="$1"; GDB="$2"; EXE="$3"
"$QEMU" -g 1234 "$EXE" &
QPID=$!
sleep 0.5
"$GDB" -q \
  -ex "set confirm off" \
  -ex "set architecture riscv:rv32" \
  -ex "target remote :1234" \
  -ex "file $EXE" \
  -ex "layout regs"
kill $QPID 2>/dev/null || true
]=])

# ---- One library per function: src/<fn>.s -> fn_<fn> ----
# Both .s and .S (the lab uses .S) are picked up. One file = one test folder:
#   src/stringfct.S -> tests/stringfct/test_*.{s,S,c}
file(GLOB FUNC_SOURCES CONFIGURE_DEPENDS
     ${CMAKE_SOURCE_DIR}/src/*.s
     ${CMAKE_SOURCE_DIR}/src/*.S)

set(ALL_TEST_TARGETS "")

foreach(f IN LISTS FUNC_SOURCES)
  get_filename_component(fn ${f} NAME_WE)
  add_library(fn_${fn} STATIC ${f})

  foreach(dep IN LISTS ${fn}_DEPS)
    target_link_libraries(fn_${fn} PUBLIC fn_${dep})
  endforeach()

  # ---- Tests for this function: tests/<fn>/test_*.{s,c} ----
  file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
       ${CMAKE_SOURCE_DIR}/tests/${fn}/test_*.s
       ${CMAKE_SOURCE_DIR}/tests/${fn}/test_*.S
       ${CMAKE_SOURCE_DIR}/tests/${fn}/test_*.c)

  set(FN_TEST_TARGETS "")
  foreach(src IN LISTS TEST_SOURCES)
    get_filename_component(tname ${src} NAME_WE)
    set(target ${fn}_${tname})            # unique across all functions

    add_executable(${target} ${src} $<TARGET_OBJECTS:start>)
    target_link_libraries(${target} PRIVATE fn_${fn})

    add_test(NAME ${fn}.${tname} COMMAND ${QEMU} $<TARGET_FILE:${target}>)
    set_tests_properties(${fn}.${tname} PROPERTIES LABELS ${fn})

    # Debug one test: cmake --build build --target debug_<fn>_<tname>
    if(GDB)
      add_custom_target(debug_${target}
        COMMAND sh ${CMAKE_BINARY_DIR}/debug.sh ${QEMU} ${GDB} $<TARGET_FILE:${target}>
        DEPENDS ${target}
        USES_TERMINAL
        VERBATIM)
    endif()

    list(APPEND FN_TEST_TARGETS ${target})
  endforeach()

  # Build + run only this function's tests
  if(FN_TEST_TARGETS)
    add_custom_target(check_${fn}
      COMMAND ${CMAKE_CTEST_COMMAND} -L "^${fn}$" --output-on-failure
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      DEPENDS ${FN_TEST_TARGETS}
      USES_TERMINAL
      VERBATIM)
    list(APPEND ALL_TEST_TARGETS ${FN_TEST_TARGETS})
  endif()
endforeach()

# Build + run everything
add_custom_target(check
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS ${ALL_TEST_TARGETS}
  USES_TERMINAL
  VERBATIM)
