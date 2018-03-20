#============================================================
# This file contains functions to help set tests.
#============================================================


# Generate a test from a folder containing a Makefile and specific sub folders.
# - These are used for application and module tests.
function(add_makefile_test_folder folder prefix_name)

    # For convenience, quietly ignore Makefiles that get passed in instead of folders.
    get_filename_component(subName ${folder} NAME)
    if("${subName}" STREQUAL "Makefile")
      return()
    endif()

    # Figure out the input, output, and truth paths
    file(RELATIVE_PATH relPath ${CMAKE_SOURCE_DIR} ${folder})
    set(dataDir   $ENV{ISIS3TESTDATA}/isis/${relPath})
    set(inputDir  ${dataDir}/input)
    set(truthDir  ${dataDir}/truth)
    set(makeFile  ${folder}/Makefile)

    # TODO: Improve variable name (from top level file)
    # The output folder may be in a different directory

    set(outputDir ${testOutputDir}/${relPath}/output)

    # Define the name CTest will use to refer to this test.
    set(testName  ${prefix_name}_test_${subName})

    ## Some tests don't need an input folder but the others must exist
    #if(NOT EXISTS ${makeFile})
    #  message(FATAL_ERROR "Required file does not exist: ${makeFile}")
    #endif()
    #if(NOT EXISTS ${truthDir})
    #  message(FATAL_ERROR "Required data folder does not exist: ${truthDir}")
    #endif()

    # Call lower level function to finish adding the test.
    add_makefile_test_target(${testName} ${makeFile} ${inputDir} ${outputDir} ${truthDir})
endfunction()


# Add a Makefile based test to the CMake test list.
macro(add_makefile_test_target testName makeFile inputDir outputDir truthDir)

  set(thisFolder "${PROJECT_SOURCE_DIR}/cmake")
  # Set up a cmake script which will execute the command in the makefile
  #  and then check the results against the truth folder.
  add_test(NAME ${testName}
           COMMAND ${CMAKE_COMMAND}
           -DMAKEFILE=${makeFile}
           -DCMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}
           -DCODE_ROOT=${PROJECT_SOURCE_DIR}
           -DDATA_ROOT=$ENV{ISIS3DATA}
           -DINPUT_DIR=${inputDir}
           -DOUTPUT_DIR=${outputDir}
           -DTRUTH_DIR=${truthDir}
           -DBIN_DIR=${CMAKE_BINARY_DIR}/bin
           -P ${thisFolder}/RunMakeFileTest.cmake)

endmacro()


# Add a class based unit test with an executable and a truth file.
macro(add_unit_test_target testFile truthFile)

  set(thisFolder "${PROJECT_SOURCE_DIR}/cmake")
  set(fullTestPath "${CMAKE_BINARY_DIR}/unitTest/${testFile}") # The binary that the script will execute

  # Set up a cmake script which will run the executable
  #  and then check the results against the truth file.
  set(testName ${testFile})
  add_test(NAME ${testName}
           COMMAND ${CMAKE_COMMAND}
           -DTEST_PROG=${fullTestPath}
           -DTRUTH_FILE=${truthFile}
           -DDATA_ROOT=$ENV{ISIS3DATA}
           -DCODE_ROOT=${PROJECT_SOURCE_DIR}
           -P ${thisFolder}/RunUnitTest.cmake)
endmacro()
