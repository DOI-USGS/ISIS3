cmake_minimum_required(VERSION 3.10)
project(sensorutilities VERSION 0.0.1 DESCRIPTION "SensorUtilities library")

set(CMAKE_CXX_STANDARD 11)

include(GNUInstallDirs)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
endif()

set(SENSOR_UTILITIES_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SENSOR_UTILITIES_INSTALL_INCLUDE_DIR "include/sensorutilities")
set(SENSOR_UTILITIES_HEADER_FILES "${SENSOR_UTILITIES_INCLUDE_DIR}/SensorUtilities.h"
                                  "${SENSOR_UTILITIES_INCLUDE_DIR}/MathUtils.h")

add_library(sensorutilities SHARED
            "${CMAKE_CURRENT_SOURCE_DIR}/src/MathUtils.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/SensorUtilities.cpp")

target_include_directories(sensorutilities
                           PUBLIC
                           $<BUILD_INTERFACE:${SENSOR_UTILITIES_INCLUDE_DIR}>
                           $<INSTALL_INTERFACE:include>)

install(FILES ${SENSOR_UTILITIES_HEADER_FILES} DESTINATION ${SENSOR_UTILITIES_INSTALL_INCLUDE_DIR})
install(TARGETS sensorutilities
        EXPORT sensorutilitiesConfig
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION include)
export(TARGETS sensorutilities
       NAMESPACE sensorutilities::
       FILE "${CMAKE_CURRENT_BINARY_DIR}/sensorutilitiesConfig.cmake")
install(EXPORT sensorutilitiesConfig
        NAMESPACE sensorutilities::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})


option (SENSOR_UTILITIES_BUILD_TESTS "Build the tests for the SensorUtilities Library" ON)

if(SENSOR_UTILITIES_BUILD_TESTS)

  enable_testing()

  add_executable(SensorUtilitiesTests
                 "${CMAKE_CURRENT_SOURCE_DIR}/tests/MathUtilsTests.cpp"
                 "${CMAKE_CURRENT_SOURCE_DIR}/tests/SensorUtilitiesTests.cpp")

  target_link_libraries(SensorUtilitiesTests
                        gmock_main
                        sensorutilities)

  include(GoogleTest)
  gtest_discover_tests(SensorUtilitiesTests
                       TEST_PREFIX SensorUtilities.)
else()
  message(STATUS "Skipping Tests")
endif()