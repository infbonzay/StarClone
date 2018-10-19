# - Try to find mpqlib
# Once done, this will define
#
#  StormLib_FOUND - system has StormLiblib
#  StormLib_INCLUDE_DIRS - the StormLiblib include directories
#  StormLib_LIBRARIES - link these to use StormLiblib

# Use pkg-config to get hints about paths
find_package(PkgConfig)

pkg_check_modules(StormLib StormLib>=7.0.0)
