cmake_minimum_required(VERSION 3.10)

project (core VERSION 1.0.0)
option(buildStaticCore "Build libisis static as well as dynamic"        OFF )
option (BUILD_CORE_TESTS "Build core module tests" ON)
option(ISIS_BUILD_SWIG "Turn on to build swig wrappers"                 OFF )

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Short and long name of this package
set(PACKAGE            "CORE")
set(PACKAGE_NAME       "USGS ISIS PVL CORE")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(Pvl_source_files ${PROJECT_SOURCE_DIR}/src/ArraySubscriptNotInRange.cpp
                     ${PROJECT_SOURCE_DIR}/src/Blob.cpp
                     ${PROJECT_SOURCE_DIR}/src/FileCreate.cpp
                     ${PROJECT_SOURCE_DIR}/src/FileName.cpp
                     ${PROJECT_SOURCE_DIR}/src/FileOpen.cpp
                     ${PROJECT_SOURCE_DIR}/src/FileRead.cpp
                     ${PROJECT_SOURCE_DIR}/src/FileWrite.cpp
                     ${PROJECT_SOURCE_DIR}/src/IException.cpp
                     ${PROJECT_SOURCE_DIR}/src/IString.cpp
                     ${PROJECT_SOURCE_DIR}/src/KeywordAmbiguous.cpp
                     ${PROJECT_SOURCE_DIR}/src/KeywordBlockEndMissing.cpp
                     ${PROJECT_SOURCE_DIR}/src/KeywordBlockInvalid.cpp
                     ${PROJECT_SOURCE_DIR}/src/KeywordBlockStartMissing.cpp
                     ${PROJECT_SOURCE_DIR}/src/KeywordDuplicated.cpp
                     ${PROJECT_SOURCE_DIR}/src/KeywordNotArray.cpp
                     ${PROJECT_SOURCE_DIR}/src/KeywordNotFound.cpp
                     ${PROJECT_SOURCE_DIR}/src/KeywordUnrecognized.cpp
                     ${PROJECT_SOURCE_DIR}/src/KeywordValueBad.cpp
                     ${PROJECT_SOURCE_DIR}/src/KeywordValueExpected.cpp
                     ${PROJECT_SOURCE_DIR}/src/KeywordValueNotInList.cpp
                     ${PROJECT_SOURCE_DIR}/src/KeywordValueNotInRange.cpp
                     ${PROJECT_SOURCE_DIR}/src/MemoryAllocationFailed.cpp
                     ${PROJECT_SOURCE_DIR}/src/MissingDelimiter.cpp
                     ${PROJECT_SOURCE_DIR}/src/Preference.cpp
                     ${PROJECT_SOURCE_DIR}/src/Pvl.cpp
                     ${PROJECT_SOURCE_DIR}/src/PvlContainer.cpp
                     ${PROJECT_SOURCE_DIR}/src/PvlFormat.cpp
                     ${PROJECT_SOURCE_DIR}/src/PvlFormatPds.cpp
                     ${PROJECT_SOURCE_DIR}/src/PvlGroup.cpp
                     ${PROJECT_SOURCE_DIR}/src/PvlKeyword.cpp
                     ${PROJECT_SOURCE_DIR}/src/PvlObject.cpp
                     ${PROJECT_SOURCE_DIR}/src/PvlSequence.cpp
                     ${PROJECT_SOURCE_DIR}/src/PvlToken.cpp
                     ${PROJECT_SOURCE_DIR}/src/PvlTokenizer.cpp
                     ${PROJECT_SOURCE_DIR}/src/Table.cpp
                     ${PROJECT_SOURCE_DIR}/src/TableField.cpp
                     ${PROJECT_SOURCE_DIR}/src/TableRecord.cpp)
# Define a target
add_library(core SHARED "${Pvl_source_files}")

target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR}/include)

find_package(Qt6 REQUIRED COMPONENTS
                                    Core
                                    Core5Compat
                                    Concurrent
                                    Widgets
                                    Xml
                                    Network)
find_package(nlohmann_json REQUIRED)
find_package(GDAL          REQUIRED CONFIG)


target_link_libraries(core PUBLIC
                                  Qt6::Core
                                  Qt6::Core5Compat
                                  Qt6::Concurrent
                                  Qt6::Widgets
                                  Qt6::Network
                                  Qt6::Xml
                           PRIVATE
                                  nlohmann_json::nlohmann_json
                                  GDAL::GDAL)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/isis
        FILES_MATCHING PATTERN "*.h")

install(TARGETS core DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../../isis/IsisPreferences
        DESTINATION ${CMAKE_INSTALL_DATADIR}/isis)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../../LICENSE.md
        DESTINATION ${CMAKE_INSTALL_DATADIR}/isis)

if(BUILD_CORE_TESTS)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../../isis/TestPreferences DESTINATION ${CMAKE_INSTALL_DATADIR}/isis)
  enable_testing()
  add_subdirectory(tests)
endif()
if(ISIS_BUILD_SWIG)
  add_subdirectory(swig)
endif()
