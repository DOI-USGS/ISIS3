cmake_minimum_required(VERSION 3.10)

add_dependencies(isis3 isis3)

file(GLOB test_source "${CMAKE_SOURCE_DIR}/tests/*.cpp")



# Link runISISTests with what we want to test and the GTest and pthread library
add_executable(runISISTests
               IsisTestMain.cpp
               ${test_source})

#message("All libraries being linked against:  ")
#message("${ALLLIBS}")               
               
target_link_libraries(runISISTests isis3 ${ALLLIBS} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} hayabusa2 pthread)

gtest_discover_tests(runISISTests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

message(STATUS "${GTEST_LIBRARIES}")
