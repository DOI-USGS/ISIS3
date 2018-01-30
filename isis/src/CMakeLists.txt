#===============================================================================

# Each of these calls will add one library plus multiple apps and unit tests.
# - The arguments are: (library_name, [list of folders needed for library])

if(${buildCore})
  add_isis_module(${CORE_LIB_NAME} base database control qisis system)

  # This folder is for under-development test code so if it is
  #  missing just ignore it.
  # - TODO: The install behavior should be disabled for this folder!
  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/local)
    message("Detected optional directory 'local'")
    add_isis_module(local local)
  endif()

endif()

if(${buildMissions})
  add_isis_module(apollo apollo)
  add_isis_module(cassini cassini)
  add_isis_module(chandrayaan1 chandrayaan1)
  add_isis_module(clementine clementine)
  add_isis_module(dawn dawn)
  add_isis_module(galileo galileo)
  add_isis_module(hayabusa hayabusa)
  add_isis_module(kaguya kaguya)
  add_isis_module(lo lo)
  add_isis_module(lro lro)
  add_isis_module(mariner mariner)
  add_isis_module(mer mer)
  add_isis_module(messenger messenger)
  add_isis_module(mex mex)
  add_isis_module(mgs mgs)
  add_isis_module(mro mro)
  add_isis_module(near near)
  add_isis_module(newhorizons newhorizons)
  add_isis_module(odyssey odyssey)
  add_isis_module(rolo rolo)
  add_isis_module(rosetta rosetta)
  add_isis_module(socet socet)
  add_isis_module(viking viking)
  add_isis_module(voyager voyager)
endif()
