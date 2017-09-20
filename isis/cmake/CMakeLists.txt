
# This file contains everything that should be exectuted AFTER the installation
# step has completed.

message("Setting up post-install behavior...")

# Set up format version numbers for the main shared library on install
install(CODE "EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_BINARY_DIR}/lib/libisis3${SO} ${CMAKE_INSTALL_PREFIX}/lib/libisis3.5.0${SO})")
install(CODE "EXECUTE_PROCESS(COMMAND ln -sf libisis3.5.0${SO} ${CMAKE_INSTALL_PREFIX}/lib/libisis3.5${SO})")
install(CODE "EXECUTE_PROCESS(COMMAND ln -sf libisis3.5${SO} ${CMAKE_INSTALL_PREFIX}/lib/libisis3${SO})")
install(CODE "EXECUTE_PROCESS(COMMAND ln -sf libisis3${SO} ${CMAKE_INSTALL_PREFIX}/lib/libisis${SO})")

# On OSX, need to correct all the paths encoded in each of the distributed library files so
#  that they properly find the distruted files using relative paths.
if (APPLE)
  # Call the correction script on each folder containing libraries
  install(CODE "EXECUTE_PROCESS(COMMAND python ${CMAKE_SOURCE_DIR}/scripts/finalizeInstalledOsxRpaths.py ${CMAKE_INSTALL_PREFIX}/lib)")
  install(CODE "EXECUTE_PROCESS(COMMAND python ${CMAKE_SOURCE_DIR}/scripts/finalizeInstalledOsxRpaths.py ${CMAKE_INSTALL_PREFIX}/3rdParty/lib resetRpath)")
  install(CODE "EXECUTE_PROCESS(COMMAND python ${CMAKE_SOURCE_DIR}/scripts/finalizeInstalledOsxRpaths.py ${CMAKE_INSTALL_PREFIX}/bin)")

  # Also need to get the plugin folders
  get_subdirectory_list(${CMAKE_SOURCE_DIR}/3rdParty/plugins pluginFolders)
  foreach(f ${pluginFolders})
    get_filename_component(name ${f} NAME)
    install(CODE "EXECUTE_PROCESS(COMMAND python ${CMAKE_SOURCE_DIR}/scripts/finalizeInstalledOsxRpaths.py ${CMAKE_INSTALL_PREFIX}/3rdParty/plugins/${name} resetRpath)")
  endforeach()

endif()
