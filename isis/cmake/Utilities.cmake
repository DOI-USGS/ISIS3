#==================================================================================
# This file contains small utility functions
#==================================================================================

# Copy one file
function(copy_file src dest)
  configure_file(${src} ${dest} COPYONLY)
endfunction()

# Copy one folder
function(copy_folder src dest)
  execute_process(COMMAND cp -r ${src} ${dest})
endfunction()

# Copy all files matching a wildcard to the output folder.
function(copy_wildcard wildcard outputFolder)
  file(GLOB files ${wildcard})
  file(COPY ${files} DESTINATION ${outputFolder})
endfunction()

# Copy all input files to the output folder
function(copy_files_to_folder files folder)
  foreach(f ${files})
    get_filename_component(filename ${f} NAME)
    set(outputPath "${folder}/${filename}")
    configure_file(${f} ${outputPath} COPYONLY)
  endforeach()
endfunction()

# Quit if the file does not exist
function(verify_file_exists path)
  if(NOT EXISTS ${path})
    message( FATAL_ERROR "Required file ${path} does not exist!" )
  endif()
endfunction()

# Set result to ON if the file contains "s", OFF otherwise.
function(file_contains path s result)
  file(READ ${path} contents)
  string(FIND "${contents}" "${s}" position)
  set(${result} ON PARENT_SCOPE)
  if(${position} EQUAL -1)
    set(${result} OFF PARENT_SCOPE)
  endif()
endfunction()


# Set result to a list of all the subdirectories in the given directory.
function(get_subdirectory_list curdir result)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    # Skip files and hidden folders.
    string(SUBSTRING ${child} 0 1 firstChar)
    if( (IS_DIRECTORY ${curdir}/${child}) AND (NOT ${firstChar} STREQUAL ".") )
      list(APPEND dirlist ${curdir}/${child})
    endif()
  endforeach()
  set(${result} ${dirlist} PARENT_SCOPE)
endfunction()

# Append the contents of IN_FILE to the end of OUT_FILE
function(cat inFile outFile)

  # If the output file does not exist, init with an empty file.
  if(NOT EXISTS "${outFile}")
    file(WRITE ${outFile} "")
  endif()

  # Perform the file concatenation.
  if(EXISTS "${inFile}")
    file(READ ${inFile} contents)
    file(APPEND ${outFile} "${contents}")
  endif()
endfunction()

# Get the correct location to generate code for items in a given input folder
# - Generated code includes uic, moc, and protobuf files.
function(get_code_gen_dir inputFolder codeGenFolder)
  file(RELATIVE_PATH relPath ${PROJECT_SOURCE_DIR} ${inputFolder})
  string(REPLACE "src" "objects" relPath ${relPath})
  set(${codeGenFolder} "${PROJECT_BINARY_DIR}/${relPath}" PARENT_SCOPE)
  file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/${relPath}")

  # Also add this folder to the include path
  # include_directories("${PROJECT_BINARY_DIR}/${relPath}")
endfunction()

# Determine the text string used to describe this OS version
function(get_os_version text)

  if(UNIX AND NOT APPLE)

    # Fetch OS information
    execute_process(COMMAND cat "/etc/os-release"
                    RESULT_VARIABLE code
                    OUTPUT_VARIABLE result
                    ERROR_VARIABLE result)
    if ("${code}" STREQUAL "0")
      # Extract OS name and version from generic Linux system
      string(REGEX MATCH "NAME=[A-Za-z\"]+" name "${result}")
      string(REGEX MATCH "VERSION_ID=[0-9\\.\"]+" version "${result}")
      string(SUBSTRING ${name} 5 -1 name)
      string(SUBSTRING ${version} 11 -1 version)
      string(REPLACE "\"" "" name ${name})
      string(REPLACE "\"" "" version ${version})
      string(REPLACE "." "_" version ${version})
    else()
      # Try the Red Hat specific command.
      execute_process(COMMAND cat "/etc/redhat-release"
                      RESULT_VARIABLE code
                      OUTPUT_VARIABLE result
                      ERROR_VARIABLE result)
      if ("${code}" STREQUAL "0")
        # Extract OS name and version from Red Hat Linux system
        string(REGEX MATCH "[0-9\\.]+" version "${result}")
        set(name RedHatEnterprise) # This part is easy
      else()
        # TODO: Test!
        # Try another command
        execute_process(COMMAND cat "/etc/lsb-release"
                        RESULT_VARIABLE code
                        OUTPUT_VARIABLE result
                        ERROR_VARIABLE result)

        message("code = ${code}")
        message("result = ${result}")

        if ("${code}" STREQUAL "0")
          # Extract OS name and version
          string(REGEX MATCH "Description:[ A-Za-z0-9\\.]+" version "${result}") # Get the line
          string(REPLACE "release"      "" version ${version}) # Strip unwanted text
          string(REPLACE " "            "" version ${version})
          string(REPLACE "Description:" "" version ${version})
          set(name "") # Included in version
        else()
          # TODO: Test!
          # Try the debian specific command
          execute_process(COMMAND cat "/etc/debian_version"
                          RESULT_VARIABLE code
                          OUTPUT_VARIABLE result
                          ERROR_VARIABLE result)

          message("code = ${code}")
          message("result = ${result}")
          if ("${code}" STREQUAL "0")
            set(version "${result}")
            set(name Debian)
          else()

            message( FATAL_ERROR "Did not recognize UNIX operating system!" )

          endif()
        endif()
      endif()
    endif()

    #message("name = ${name}")
    #message("version = ${version}")

    set(prefix "Linux_x86_64_")

  # Build the final output string
  elseif(APPLE)

    # Fetch OS information
    execute_process(COMMAND sw_vers
                    OUTPUT_VARIABLE result
                    ERROR_VARIABLE result)

    # Format the string
    string(REGEX MATCH "[0-9]+\.[0-9]+\.?[0-9]*" version "${result}")
    string(REGEX MATCH "^[0-9]+.[0-9]+" version "${version}")
    string(REPLACE "." "_" version "${version}")

    set(name   "MacOSX")
    set(prefix "Darwin_x86_64_")

  else()
    message( FATAL_ERROR "Did not recognize a supported operating system!" )
  endif()

  # Final string assembly
  set(${text} ${prefix}${name}${version} PARENT_SCOPE)
endfunction()


# Delete the first N lines of a file
function(apply_skiplines path number)

  if(${number} EQUAL 0)
    return()
  endif()

  # The first line counts as line 1 for the tail command
  MATH(EXPR number "${number}+1")

  set(temp ${path}_temp)
  file(RENAME ${path} ${temp})
  message("tail -n +${number} ${temp} OUTPUT_FILE ${path}")
  execute_process(COMMAND tail -n +${number} ${temp} OUTPUT_FILE ${path})
  #file(REMOVE ${temp})
endfunction()

# Strip all lines beginning with one of the words
function(apply_ignorelines path words)

  set(temp ${path}_temp)
  file(RENAME ${path} ${temp})

  #Set up special grep command to remove these lines
  message("words = ${words}")
  string(REPLACE " " "|" fullS "${words}")

  message("COMMAND grep -vEw ${fullS} ${temp}")
  execute_process(COMMAND grep -vEw ${fullS} ${temp} OUTPUT_FILE ${path})

  #file(REMOVE ${temp})
endfunction()


#------------------------------------------------------------

# Wrapper function to add a library and its components
function(add_library_wrapper name sourceFiles libDependencies)

  # The only optional argument is "alsoStatic", which indicates that
  #  the library should be build both shared and static.
  set(alsoStatic ${ARGN})

  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

  # Add library, set dependencies, and add to installation list.
  add_library(${name} SHARED ${sourceFiles})
  set_target_properties(${name} PROPERTIES LINKER_LANGUAGE CXX)
  target_link_libraries(${name} ${libDependencies})
  install(TARGETS ${name} DESTINATION lib)

  # buildStaticCore is a command line option specified in the top CMakeLists.txt file.
  if(alsoStatic AND ${buildStaticCore})
    # The static version needs a different name, but in the end the file
    # needs to have the same name as the shared lib.
    set(staticName "${name}_static")
    message("Adding static library ${staticName}")

    add_library("${staticName}" STATIC ${sourceFiles})
    set_target_properties(${staticName} PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(${staticName} ${libDependencies})

    # Use a copy -> install combo to get the file to the correct place.
    add_custom_command(TARGET ${staticName} POST_BUILD
                       COMMAND mv ${CMAKE_BINARY_DIR}/src/lib${staticName}.a
                                  ${CMAKE_BINARY_DIR}/src/lib${name}.a)

    install(CODE "EXECUTE_PROCESS(COMMAND cp ${CMAKE_BINARY_DIR}/lib/lib${name}.a
                                             ${CMAKE_INSTALL_PREFIX}/lib/lib${name}.a)")
  endif()

endfunction()

function(get_version libFile returnVar)
  set(${returnVar} "")
  set(FOUND_VERSION "")
  # Get the File name
  get_filename_component(VERSION_FILE ${libFile} NAME_WE)
  # message("VERSION_FILE = ${VERSION_FILE}")
  # Get the path to the dylib file as the so with version info is in the same area
  string(REGEX MATCH "^/(([a-z or A-Z or 0-9])*/)*" PATH_TO_VERSION ${libFile})
  # message("PATH_TO_VERSION= ${PATH_TO_VERSION}")
  # Glob for the dylib file with the version number
  # message("CURRENT PREFIX = ${PATH_TO_VERSION}${VERSION_FILE}")
  file(GLOB FOUND_FILES
        "${PATH_TO_VERSION}${VERSION_FILE}-[0-9].[0-9].[0-9]*"
        "${PATH_TO_VERSION}${VERSION_FILE}-[0-9].[0-9]*"
        "${PATH_TO_VERSION}${VERSION_FILE}-3.1.so"
        "${PATH_TO_VERSION}${VERSION_FILE}.[0-9].[0-9].[0-9]*"
        "${PATH_TO_VERSION}${VERSION_FILE}_[0-9].[0-9].[0-9]*"
        "${PATH_TO_VERSION}${VERSION_FILE}.so.[0-9]*.[0-9]*.[0-9]*"
        "${PATH_TO_VERSION}${VERSION_FILE}.so.[0-9]*.[0-9]*"
        "${PATH_TO_VERSION}${VERSION_FILE}.[0-9].dylib"
        "${PATH_TO_VERSION}${VERSION_FILE}.[0-9]*.[0-9]*.dylib"
      )
  # message("FOUND_FILES = ${FOUND_FILES}")
  foreach(f ${FOUND_FILES})
    # Ideally glob found a single file and grep for the version number found
    get_filename_component(VERSION_FILE ${f} NAME)

    string(REGEX MATCH "[\\.,-][0-9]+\\.[0-9]+\\.[0-9]+" CURR_VERSION ${VERSION_FILE})

    if(NOT CURR_VERSION)
      string(REGEX MATCH "[\\.,-][0-9]+\\.[0-9]+" CURR_VERSION ${VERSION_FILE})
    endif(NOT CURR_VERSION)


    if(NOT CURR_VERSION)
      string(REGEX MATCH "[\\.,-][0-9]+\\." CURR_VERSION ${VERSION_FILE})
      string(SUBSTRING ${CURR_VERSION} 0 2 CURR_VERSION)
    endif(NOT CURR_VERSION)

    string(SUBSTRING ${CURR_VERSION} 1 -1 CURR_VERSION)

    if(FOUND_VERSION)
      # message("VERSION = ${CURR_VERSION}")
      # message("FOUND_VERSION = ${FOUND_VERSION}")
      if(${FOUND_VERSION} VERSION_LESS ${CURR_VERSION})
        set(FOUND_VERSION ${CURR_VERSION})
      endif()
    else(FOUND_VERSION)
      set(FOUND_VERSION ${CURR_VERSION})
    endif(FOUND_VERSION)

  endforeach()
  set(${returnVar} ${FOUND_VERSION} PARENT_SCOPE)
endfunction()
