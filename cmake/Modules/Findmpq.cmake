# - Try to find mpqlib
# Once done, this will define
#
#  mpq_FOUND - system has mpqlib
#  mpq_INCLUDE_DIRS - the mpqlib include directories
#  mpq_LIBRARIES - link these to use mpqlib

# Use pkg-config to get hints about paths
find_package(PkgConfig)

pkg_check_modules(mpq mpq>=0.5.3)
