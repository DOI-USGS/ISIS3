#===========================================================================
# Code for installing the third part libraries to the output folder.
#===========================================================================

# Library portion of the installation
function(install_third_party_libs)



  # Where all the library files will go
  set(installLibFolder "${CMAKE_INSTALL_PREFIX}/3rdParty/lib")

  # TEMPORARY CODE TO INSTALL ALL FILES FROM V006/lib into 3rdParty/lib 
  install(DIRECTORY "/usgs/pkgs/local/v007/lib" DESTINATION ${CMAKE_INSTALL_PREFIX})
  
  # Loop through all the library files in our list
  foreach(library ${ALLLIBS})
    get_filename_component(extension ${library} EXT)
    if ("${extension}" STREQUAL ".so" OR "${extension}" STREQUAL ".dylib" )
      get_filename_component(librarypath ${library} PATH)

      # Copy file to output directory
      file(RELATIVE_PATH relPath "${thirdPartyDir}/lib" ${library})

      # Check if the file is a symlink
      execute_process(COMMAND readlink ${library} OUTPUT_VARIABLE link)

      message(STATUS "${library}")
      message("LIBRARY = ${library}")

      if ("${link}" STREQUAL "")
        # Copy original files and framework folders
        if(IS_DIRECTORY ${library})
          install(DIRECTORY ${library} DESTINATION ${installLibFolder})
        else()
          install(PROGRAMS ${library} DESTINATION ${installLibFolder})
        endif()

      else()
        # Loop through possible chains of namelinks (i.e. lib.so -> lib.so.3.5 -> lib.so.3.5.1)
        while (NOT "${link}" STREQUAL "")
          # Recreate symlinks
          string(REGEX REPLACE "\n$" "" link "${link}") # Strip trailing newline
          install(CODE "EXECUTE_PROCESS(COMMAND ln -fs ${link} ${installLibFolder}/${relPath})")
          install(PROGRAMS "${librarypath}/${link}" DESTINATION ${installLibFolder})
          # Set next iteration of possible symlinks
          set(library "${librarypath}/${link}")
          file(RELATIVE_PATH relPath "${thirdPartyDir}/lib" ${library})
          execute_process(COMMAND readlink ${library} OUTPUT_VARIABLE link)
        endwhile()
      endif()
    endif()
  endforeach()

  # Copy over QT Frameworks
  if(APPLE)
    # execute_process(COMMAND cp -Lr ${library} ${installLibFolder})
  endif(APPLE)
endfunction()



# Plugin portion of the installation
function(install_third_party_plugins)

  # Where all the plugin files will go
  set(installPluginFolder "${CMAKE_INSTALL_PREFIX}/3rdParty/plugins")

  # Copy all of the plugin files
   foreach(plugin ${THIRDPARTYPLUGINS})
    file(RELATIVE_PATH relPath "${thirdPartyDir}/plugins" ${plugin})
    get_filename_component(relPath ${relPath} DIRECTORY) # Strip filename
    install(PROGRAMS ${plugin} DESTINATION ${installPluginFolder}/${relPath})
  endforeach()

endfunction()

# License portion of the installation
function(install_third_party_license)
  # Specify top level directories
  if(APPLE)
    set(LIC_DIR "/opt/usgs/v007/3rdParty/license")
  else()
    set(LIC_DIR "/usgs/pkgs/local/v007/license")
  endif()
  if(NOT EXISTS ${CMAKE_INSTALL_PREFIX}/3rdParty)
    execute_process(COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/3rdParty/)
  endif()
  execute_process(COMMAND cp -r ${LIC_DIR} ${CMAKE_INSTALL_PREFIX}/3rdParty/license)

endfunction()


# Install all third party libraries and plugins
function(install_third_party)

  # The files are available pre-build but are not copied until make-install is called.
  message("Setting up 3rd party lib installation...")
  install_third_party_libs()

  message("Setting up 3rd party plugin installation...")
  install_third_party_plugins()

  message("Obtaining licenses...")
  install_third_party_license()

  # Finish miscellaneous file installation
  install(FILES "${CMAKE_SOURCE_DIR}/3rdParty/lib/README"
          DESTINATION ${CMAKE_INSTALL_PREFIX}/3rdParty/lib)

endfunction()
