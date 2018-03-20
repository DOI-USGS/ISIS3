#===========================================================================
# Code for installing the third part libraries to the output folder.
#===========================================================================

# Library portion of the installation
function(install_third_party_libs)

  # Where all the library files will go
  set(installLibFolder "${CMAKE_INSTALL_PREFIX}/3rdParty/lib")
  execute_process(COMMAND mkdir -p ${installLibFolder})

  # Loop through all the library files in our list
  foreach(library ${ALLLIBS})
    get_filename_component(extension ${library} EXT)
    if ("${extension}" STREQUAL ".so" OR "${extension}" STREQUAL ".dylib" )
      #get path to library in libararypath
      get_filename_component(librarypath ${library} PATH)

      # Copy file to output directory
      file(RELATIVE_PATH relPath "${thirdPartyDir}/lib" ${library})

      # Check if the file is a symlink
      #execute_process(COMMAND readlink ${library} OUTPUT_VARIABLE link)
      message(STATUS "${library}")
      execute_process(COMMAND cp -L ${library} ${installLibFolder})
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
    file(RELATIVE_PATH relPath "${PLUGIN_DIR}" ${plugin})
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
    install(CODE "execute_process(COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/3rdParty/)")
  endif()
  install(CODE "execute_process(COMMAND cp -r ${LIC_DIR} ${CMAKE_INSTALL_PREFIX}/3rdParty/license)")

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
  file(WRITE "${CMAKE_INSTALL_PREFIX}/3rdParty/lib/README" "This directory contains O/S and hardware specific shared libraries needed\nto execute ISIS applications")

endfunction()
