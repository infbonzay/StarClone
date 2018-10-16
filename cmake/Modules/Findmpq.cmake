# - Try to find mpqlib
# Once done, this will define
#
#  mpq_FOUND - system has mpqlib
#  mpq_INCLUDE_DIRS - the mpqlib include directories
#  mpq_LIBRARIES - link these to use mpqlib

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(mpq_PKGCONF mpq)

# Main include dir
find_path(mpq_INCLUDE_DIR
  NAMES StormLib.h
  PATHS ${mpq_PKGCONF_INCLUDE_DIRS}
  PATH_SUFFIXES mpq
)

# Finally the library itself
find_library(mpq_LIBRARY
  NAMES mpq
  PATHS ${mpq_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(mpq_PROCESS_INCLUDES mpq_INCLUDE_DIR mpqConfig_INCLUDE_DIR)
set(mpq_PROCESS_LIBS mpq_LIBRARY)
libfind_process(mpq)
