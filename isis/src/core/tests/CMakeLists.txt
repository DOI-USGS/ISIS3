cmake_minimum_required(VERSION 3.10)

find_package(Threads)

include(../cmake/gtest.cmake)
include(GoogleTest)

set(test_source ${CMAKE_SOURCE_DIR}/tests/ConstantsTests.cpp
                ${CMAKE_SOURCE_DIR}/tests/IsisCoreTestMain.cpp)

# Link runISISTests with what we want to test and the GTest and pthread library
add_executable(runISISCoreTests
               ${test_source})

target_link_libraries(runISISCoreTests core gtest Threads::Threads)

gtest_discover_tests(runISISCoreTests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)
