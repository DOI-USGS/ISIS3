# This file contains everything that should be exectuted AFTER the installation
# step has completed.

message("Setting up post-install behavior...")

# Set up format version numbers for the main shared library on install
install(CODE "EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_BINARY_DIR}/lib/libisis${SO} ${CMAKE_INSTALL_PREFIX}/lib/libisis${VERSION}${SO})")
install(CODE "EXECUTE_PROCESS(COMMAND ln -sf libisis${VERSION}${SO} ${CMAKE_INSTALL_PREFIX}/lib/libisis${VERSION_MAJOR}.${VERSION_MINOR}${SO})")
install(CODE "EXECUTE_PROCESS(COMMAND ln -sf libisis${VERSION_MAJOR}.${VERSION_MINOR}${SO} ${CMAKE_INSTALL_PREFIX}/lib/libisis${VERSION_MAJOR}${SO})")
install(CODE "EXECUTE_PROCESS(COMMAND ln -sf libisis${VERSION_MAJOR}${SO} ${CMAKE_INSTALL_PREFIX}/lib/libisis${SO})")

# On OSX, need to correct all the paths encoded in each of the distributed library files so
#  that they properly find the distruted files using relative paths.
if (APPLE)
  # Also need to get the plugin folders
  get_subdirectory_list(${CMAKE_SOURCE_DIR}/3rdParty/plugins pluginFolders)
  foreach(f ${pluginFolders})
    get_filename_component(name ${f} NAME)
    install(CODE "EXECUTE_PROCESS(COMMAND python ${CMAKE_SOURCE_DIR}/scripts/finalizeInstalledOsxRpaths.py ${CMAKE_INSTALL_PREFIX}/3rdParty/plugins/${name} resetRpath)")
  endforeach()
endif()
