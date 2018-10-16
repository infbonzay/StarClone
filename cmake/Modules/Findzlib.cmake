# - Try to find zliblib
# Once done, this will define
#
#  zlib_FOUND - system has zliblib
#  zlib_INCLUDE_DIRS - the zliblib include directories
#  zlib_LIBRARIES - link these to use zliblib

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(zlib_PKGCONF zlib)

# Main include dir
find_path(zlib_INCLUDE_DIR
  NAMES zlib.h
  PATHS ${zlib_PKGCONF_INCLUDE_DIRS}
  PATH_SUFFIXES zlib
)

# Finally the library itself
find_library(zlib_LIBRARY
  NAMES z
  PATHS ${zlib_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(zlib_PROCESS_INCLUDES zlib_INCLUDE_DIR zlibConfig_INCLUDE_DIR)
set(zlib_PROCESS_LIBS zlib_LIBRARY)
libfind_process(zlib)
