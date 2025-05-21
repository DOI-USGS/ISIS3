#============================================================================
# Script to read in a MakeFile based test and run it without relying on any
# of the old Makefile infrastructure.
#============================================================================

# Minimum version 3.5 required to avoid cmake warnings
cmake_minimum_required(VERSION 3.15)
list(APPEND CMAKE_MODULE_PATH "${CODE_ROOT}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CODE_ROOT}/cmake")
include(Utilities)

# Function to run the test and check the results
function(run_app_makefile_test makefile inputFolder outputFolder truthFolder binFolder)

  # Build the test name
  get_filename_component(sourceFolder ${makefile}     DIRECTORY)
  set(appName ${appName}_${testName})

  # Check if there are copies of the input/truth folders in the source folder,
  #  if so use those instead of the original location.
  if(EXISTS ${sourceFolder}/input)
    set(inputFolder ${sourceFolder}/input)
  endif()
  if(EXISTS ${sourceFolder}/truth)
    set(truthFolder ${sourceFolder}/truth)
  endif()

  message("Working directory: ${sourceFolder}")
  execute_process(COMMAND make test --no-print-directory WORKING_DIRECTORY ${sourceFolder} OUTPUT_VARIABLE result)
  
  message(STATUS "Test output log:\n${result}")
  if (result MATCHES "OK")
      set(failed "OFF")
  else()
       set(failed "ON")
  endif()

  # If any file failed, the test is a failure.
  if(${failed})
    #message("TRUTH: ${TRUTH}") # This message just prints "TRUTH"; not helpful.
    message(FATAL_ERROR "Test failed.") # The test output log is printed above.
  endif()

endfunction()

#===================================================================================
# This is the main script that gets run during the test.
# - Just redirect to the main function call.

# Needed for IsisPreferences and other test data to be found
set(ENV{ISISDATA} "${DATA_ROOT}")

run_app_makefile_test(${MAKEFILE} ${INPUT_DIR} ${OUTPUT_DIR} ${TRUTH_DIR} ${BIN_DIR})
