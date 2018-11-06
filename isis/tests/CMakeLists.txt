cmake_minimum_required(VERSION 3.10)

add_dependencies(isis3 isis3)

# Link runISISTests with what we want to test and the GTest and pthread library
add_executable(runISISTests
               FileNameTests.cpp)
               
target_link_libraries(runISISTests isis3 ${ALLLIBS} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)

gtest_discover_tests(runISISTests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../tests)
