# - Try to find grplib
# Once done, this will define
#
#  grp_FOUND - system has grplib
#  grp_INCLUDE_DIRS - the grplib include directories
#  grp_LIBRARIES - link these to use grplib

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(grp_PKGCONF grp)

# Main include dir
find_path(grp_INCLUDE_DIR
  NAMES grp.h
  PATHS ${grp_PKGCONF_INCLUDE_DIRS}
  PATH_SUFFIXES grp
)

# Finally the library itself
find_library(grp_LIBRARY
  NAMES grp
  PATHS ${grp_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(grp_PROCESS_INCLUDES grp_INCLUDE_DIR grpConfig_INCLUDE_DIR)
set(grp_PROCESS_LIBS grp_LIBRARY)
libfind_process(grp)
