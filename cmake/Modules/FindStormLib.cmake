# - Try to find StormLiblib
# Once done, this will define
#
#  StormLib_FOUND - system has StormLiblib
#  StormLib_INCLUDE_DIRS - the StormLiblib include directories
#  StormLib_LIBRARIES - link these to use StormLiblib

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(StormLib_PKGCONF StormLib)

# Main include dir
find_path(StormLib_INCLUDE_DIR
  NAMES StormLib.h
  PATHS ${StormLib_PKGCONF_INCLUDE_DIRS}
  PATH_SUFFIXES StormLib
)

# Finally the library itself
find_library(StormLib_LIBRARY
  NAMES StormLib
  PATHS ${StormLib_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(StormLib_PROCESS_INCLUDES StormLib_INCLUDE_DIR StormLibConfig_INCLUDE_DIR)
set(StormLib_PROCESS_LIBS StormLib_LIBRARY)
libfind_process(StormLib)
