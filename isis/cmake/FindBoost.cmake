# CMake module for find_package(Boost)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   BOOST_INCLUDE_DIR

find_path(BOOST_INCLUDE_DIR
  NAME flyweight.hpp
  PATH_SUFFIXES "boost/boost${Boost_FIND_VERSION}/boost/" "boost"
)

get_filename_component(BOOST_ROOT_INCLUDE_DIR "${BOOST_INCLUDE_DIR}" DIRECTORY)

find_library(BOOST_ATOMIC_MT_LIBRARY
  NAMES boost_atomic-mt boost_atomic
)

find_library(BOOST_LOG_MT_LIBRARY
  NAMES boost_log-mt boost_log
)

find_library(BOOST_REGEX_MT_LIBRARY
  NAMES boost_regex-mt boost_regex
)

find_library(BOOST_LOG_SETUP_MT_LIBRARY
  NAMES boost_log_setup-mt boost_log_setup
)

find_library(BOOST_SERIALIZATION_MT_LIBRARY
  NAMES boost_serialization-mt boost_serialization
)

find_library(BOOST_CHRONO_MT_LIBRARY
  NAMES boost_chrono-mt boost_chrono
)

find_library(BOOST_MATH_C99_MT_LIBRARY
  NAMES boost_math_c99-mt boost_math_c99
)

find_library(BOOST_SIGNALS_MT_LIBRARY
  NAMES boost_signals-mt boost_signals
)

find_library(BOOST_CONTAINER_MT_LIBRARY
  NAMES boost_container-mt boost_container
)

find_library(BOOST_MATH_C99F_MT_LIBRARY
  NAMES boost_math_c99f-mt boost_math_c99f
)

find_library(BOOST_CONTEXT_MT_LIBRARY
  NAMES boost_context-mt boost_context
)

find_library(BOOST_MATH_C99L_MT_LIBRARY
  NAMES boost_math_c99l-mt boost_math_c99l
)

find_library(BOOST_SYSTEM_MT_LIBRARY
  NAMES boost_system-mt boost_system
)

find_library(BOOST_COROUTINE_MT_LIBRARY
  NAMES boost_coroutine-mt boost_coroutine
)

find_library(BOOST_MATH_TR1_MT_LIBRARY
  NAMES boost_math_tr1-mt boost_math_tr1
)

find_library(BOOST_MATH_TR1F_MT_LIBRARY
  NAMES boost_math_tr1f-mt boost_math_tr1f
)

find_library(BOOST_MATH_TR1L_MT_LIBRARY
  NAMES boost_math_tr1l-mt boost_math_tr1l
)

find_library(BOOST_TEST_EXEC_MONITOR_MT_LIBRARY
  NAMES boost_test_exec_monitor-mt boost_test_exec_monitor
)

find_library(BOOST_DATE_TIME_MT_LIBRARY
  NAMES boost_date_time-mt boost_date_time
)

find_library(BOOST_THREAD_MT_LIBRARY
  NAMES boost_thread-mt boost_thread
)

find_library(BOOST_EXCEPTION_MT_LIBRARY
  NAMES boost_exception-mt boost_exception
)

find_library(BOOST_TIMER_MT_LIBRARY
  NAMES boost_timer-mt boost_timer
)

find_library(BOOST_FILESYSTEM_MT_LIBRARY
  NAMES boost_filesystem-mt boost_filesystem
)

find_library(BOOST_PRG_EXEC_MONITOR_MT_LIBRARY
  NAMES boost_prg_exec_monitor-mt boost_prg_exec_monitor
)

find_library(BOOST_PROGRAM_OPTIONS_MT_LIBRARY
  NAMES boost_program_options-mt boost_program_options
)

find_library(BOOST_UNIT_TEST_FRAMEWORK_MT_LIBRARY
  NAMES boost_unit_test_framework-mt boost_unit_test_framework
)

find_library(BOOST_IOSTREAMS_MT_LIBRARY
  NAMES boost_iostreams-mt boost_iostreams
)

#message("BOOST_IOSTREAMS_MT_LIBRARY = ${BOOST_IOSTREAMS_MT_LIBRARY}")
#tjw:  Not sure if needed...commenting out because library is missing
#find_library(BOOST_PYTHON_MT_LIBRARY
#  NAMES boost_python-mt boost_python
#)

find_library(BOOST_WAVE_MT_LIBRARY
  NAMES boost_wave-mt boost_wave
)

find_library(BOOST_RANDOM_MT_LIBRARY
  NAMES boost_random-mt boost_random
)

find_library(BOOST_WSERIALIZATION_MT_LIBRARY
  NAMES boost_wserialization-mt boost_wserialization
)

find_library(PYTHON_LIBRARY
  NAMES python2 python2.7 python3
)

find_library(C_LIBRARY
  NAMES c
)
