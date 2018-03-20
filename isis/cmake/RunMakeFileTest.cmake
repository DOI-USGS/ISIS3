#============================================================================
# Script to read in a MakeFile based test and run it without relying on any
# of the old Makefile infrastructure.
#============================================================================

cmake_minimum_required(VERSION 3.3)
list(APPEND CMAKE_MODULE_PATH "${CODE_ROOT}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CODE_ROOT}/cmake")
include(Utilities)


# Function to run the test and check the results
function(run_app_makefile_test makefile inputFolder outputFolder truthFolder binFolder)

  # Build the test name
  get_filename_component(sourceFolder ${makefile}     DIRECTORY)
#   get_filename_component(testName     ${sourceFolder} NAME)
#   get_filename_component(folder       ${sourceFolder} DIRECTORY)
#   get_filename_component(folder       ${folder}       DIRECTORY)
#   get_filename_component(appName      ${folder}       NAME)
  set(appName ${appName}_${testName})

  # Check if there are copies of the input/truth folders in the source folder,
  #  if so use those instead of the original location.
  if(EXISTS ${sourceFolder}/input)
    set(inputFolder ${sourceFolder}/input)
  endif()
  if(EXISTS ${sourceFolder}/truth)
    set(truthFolder ${sourceFolder}/truth)
  endif()

#   # Read in the MakeFile
#   if(NOT EXISTS ${makefile})
#     message(FATAL_ERROR "App test MakeFile ${makefile} was not found!")
#   endif()
#   file(READ ${makefile} makefileContents)
#   # Replace include line with a short list of definitions
#   set(newDefinitions "INPUT=${inputFolder}\nOUTPUT=${outputFolder}\nRM=rm -f\nCP=cp\nLS=ls\nMV=mv\nSED=sed\nTAIL=tail\nECHO=echo\nCAT=cat\nLS=ls")
#   string(REPLACE "include ${CODE_ROOT}/make/isismake.tsts" "${newDefinitions}" newFileContents "${makefileContents}")
#
#   # Set required environment variables
#   set(ENV{PATH} "${binFolder}:$ENV{PATH}")
#
#   # Select the log file
#   set(logFile "${binFolder}/${appName}.output")
#   message("logFile = ${logFile}")
#
#   # Execute the Makefile we just generated
#   set(code "")
#   execute_process(COMMAND rm -rf ${outputFolder})
#   execute_process(COMMAND rm -f ${logFile})

  execute_process(COMMAND make test WORKING_DIRECTORY ${sourceFolder} OUTPUT_VARIABLE result)
  message("result: ${result}")
  if (result MATCHES "OK")
      set(failed "OFF")
  else()
       set(failed "ON")
  endif()


  # If any file failed, the test is a failure.
  if(${failed})
    message("TRUTH: ${TRUTH}")
    message(FATAL_ERROR "Test failed. Result:\n ${result}")
  endif()

endfunction()




#===================================================================================
# This is the main script that gets run during the test.
# - Just redirect to the main function call.

# Needed for IsisPreferences and other test data to be found
set(ENV{ISIS3DATA} "${DATA_ROOT}")

run_app_makefile_test(${MAKEFILE} ${INPUT_DIR} ${OUTPUT_DIR} ${TRUTH_DIR} ${BIN_DIR})
