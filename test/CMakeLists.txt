add_executable(dxbc_test dxbc_test.cpp)
target_link_libraries(dxbc_test dxbcdec)

add_test(NAME dxbc_vso COMMAND dxbc_test ${CMAKE_CURRENT_SOURCE_DIR}/test.vso)
add_test(NAME dxbc_pso COMMAND dxbc_test ${CMAKE_CURRENT_SOURCE_DIR}/test.pso)