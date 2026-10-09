# Installation of the PSMRTS system using FetchContent
include(FetchContent)

# This downloads PSMRTS into ISIS3/build/_deps
set(PSMRTS_BUILD_APPS      ON)
set(PSMRTS_BUILD_CAPI_APPS ON)
set(PSMRTS_BUILD_SHARED    ON)
FetchContent_Declare(
  psmrts         # v0.9.0
  GIT_REPOSITORY https://github.com/UA-LPL/psmrts.git
  GIT_TAG        9d1c0976e44db96bc9651d599d76541951233608 
)

# Configure the PSMRTS system
FetchContent_MakeAvailable(psmrts)
