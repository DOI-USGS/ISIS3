#==================================================================
# Contains functions for generating code files
#==================================================================

# TODO: Can we consolidate the following three functions?


# Generate ui_*.h files from *.ui files using QT tool uic.
# - ${UIC} must point to the uic tool
function( generate_ui_files uiGenOut folder)

  # Finds all .ui files in the current dir
  file(GLOB uiInput "${folder}/*.ui")

  # If no .ui files in this folder we are finished.
  list(LENGTH uiInput numFiles)
  if (${numFiles} EQUAL 0)
    set(${uiGenOut} "" PARENT_SCOPE)
    return()
  endif()

  # Set where generated files go to and add that directory to the include path
  get_code_gen_dir(${folder} uiGenDir)

  # For each input ui file
  foreach(uiFile ${uiInput})
    # Get the name of the file without extension
    get_filename_component(uiName ${uiFile} NAME_WE)

    # Add the generated file to UI_GEN variable
    set(outUiFile "${uiGenDir}/ui_${uiName}.h")
    set(uiGen      ${uiGen} ${outUiFile})

    # Add the custom command that will generate this file
    # - The generated files will be put in the CMake build directory,
    #   not the source tree
    add_custom_command(OUTPUT   ${outUiFile}
                       COMMAND  ${UIC}  ${uiFile} -o ${outUiFile} && cp ${outUiFile} ${CMAKE_SOURCE_DIR}/incs
                       DEPENDS  ${uiFile}
                       WORKING_DIRECTORY ${folder}
                       COMMENT "Generating UI headers...")
  endforeach()


  set(${uiGenOut} ${uiGen} PARENT_SCOPE) # Set up output variable

endfunction()




# Generate moc_*.cpp files from *.h files using Q_OBJECT using the moc tool.
# - ${MOC} must point to the moc tool
function( generate_moc_files mocGenOut folder)

  # Finds all .h files in the current dir including the text Q_OBJECT
  file(GLOB candidateFiles "${folder}/*.h")
  set(mocInput)
  foreach(f ${candidateFiles})
    execute_process(COMMAND grep Q_OBJECT ${f}
                    OUTPUT_VARIABLE result
                    RESULT_VARIABLE code)
    if("${code}" STREQUAL "0")
      set(mocInput ${mocInput} ${f})
    endif()
  endforeach()

  # If no Q_OBJECT files in this folder we are finished.
  list(LENGTH mocInput numFiles)
  if (${numFiles} EQUAL 0)
    set(${mocGenOut} "" PARENT_SCOPE)
    return()
  endif()

  # Set where generated files go to and add that directory to the include path
  get_code_gen_dir(${folder} mocGenDir)

  # For each input moc file
  foreach(mocFile ${mocInput})
    # Get the name of the file without extension
    get_filename_component(mocName ${mocFile} NAME_WE)

    # Add the generated file to mocGen variable
    set(outMocFile "${mocGenDir}/moc_${mocName}.cpp")
    set(mocGen       ${mocGen} ${outMocFile})

    # Add the custom command that will generate this file
    # - The generated files will be put in the CMake build directory,
    #   not the source tree
    add_custom_command(OUTPUT   ${outMocFile}
                       COMMAND  ${MOC}  ${mocFile} -o ${outMocFile}
                       DEPENDS  ${mocFile}
                       WORKING_DIRECTORY ${folder}
                       COMMENT "Generating MOC files...")
  endforeach()
  set(${mocGenOut} ${mocGen} PARENT_SCOPE) # Set up output variable
endfunction()

# Generate ProtoBuf output files for an obj folder.
# - ${PROTOC} must point to the protobuf tool
function(generate_protobuf_files protoGenOut folder)

  # Finds all .proto files in the current dir
  file(GLOB protoInput "${folder}/*.proto")

  # If no .proto files in this folder we are finished.
  list(LENGTH protoInput numFiles)
  if (${numFiles} EQUAL 0)
    set(${protoGenOut} "" PARENT_SCOPE)
    return()
  endif()

  # Set where generated files go to and add that directory to the include path
  get_code_gen_dir(${folder} protoGenDir)

  # For each input protobuf file
  foreach(protoFile ${protoInput})
    # Get the name of the file without extension
    get_filename_component(protoName ${protoFile} NAME_WE)

    # Add the two generated files to PROTO_GEN variable
    set(protoGen ${protoGen}
        ${protoGenDir}/${protoName}.pb.h
        ${protoGenDir}/${protoName}.pb.cc)

    set(PROTO_HEADERS ${PROTO_HEADERS} ${protoGenDir}/${protoName}.pb.h)
  endforeach()

  # Add the custom command that will generate all the files
  # - The generated files will be put in the CMake build directory, not the source tree.
  add_custom_command(OUTPUT   ${protoGen}
                     COMMAND  ${PROTOC} --proto_path ${folder} --cpp_out ${protoGenDir} ${protoInput} && cp ${PROTO_HEADERS} ${CMAKE_SOURCE_DIR}/incs
                     DEPENDS  ${protoInput}
                     WORKING_DIRECTORY ${folder}
                     COMMENT "Generating Protocol Buffers...")

  set(${protoGenOut} ${protoGen} PARENT_SCOPE) # Set up output variable
endfunction()
