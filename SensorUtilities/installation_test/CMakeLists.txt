cmake_minimum_required(VERSION 3.10)
project(installtest VERSION 1.0.0 DESCRIPTION "Installation test for SensorUtilities library")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)

find_package(sensorutilities REQUIRED)

add_executable(installtest main.cpp)

target_link_libraries(installtest PUBLIC sensorutilities::sensorutilities)