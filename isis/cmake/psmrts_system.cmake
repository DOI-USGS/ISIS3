# Installation of the PSMRTS system using FetchContent
include(FetchContent)

# This downloads PSMRTS into ISIS3/build/_deps
set(PSMRTS_BUILD_APPS      ON)
set(PSMRTS_BUILD_CAPI_APPS ON)
set(PSMRTS_BUILD_SHARED    ON)
FetchContent_Declare(
  psmrts
  GIT_REPOSITORY https://github.com/UA-LPL/psmrts.git
  GIT_TAG        6b44a83926351638394688534eced41838f536c1
)

# Configure the PSMRTS system
FetchContent_MakeAvailable(psmrts)
