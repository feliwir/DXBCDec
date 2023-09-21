cmake_minimum_required(VERSION 3.10)
project(DXBCDec LANGUAGES CXX)

option(DXBC_COVERAGE "Build coverage" OFF)
option(DXBC_USE_SANITIZER "Use address sanitizer to find any memory leaks" OFF)
option(DXBC_TESTING "Build tests" ON)

if(DXBC_USE_SANITIZER)
  set(CMAKE_CXX_FLAGS_DEBUG
      "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
  set(CMAKE_LINKER_FLAGS_DEBUG
      "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

if(DXBC_COVERAGE)
  set(COVERAGE_COMPILER_FLAGS "-g -fprofile-arcs -ftest-coverage")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_COMPILER_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_COMPILER_FLAGS}")
endif()

add_subdirectory(src)
if(DXBC_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
