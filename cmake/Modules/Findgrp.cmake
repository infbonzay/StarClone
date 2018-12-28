# - Try to find grplib
# Once done, this will define
#
#  grp_FOUND - system has grplib
#  grp_INCLUDE_DIRS - the grplib include directories
#  grp_LIBRARIES - link these to use grplib

# Use pkg-config to get hints about paths
find_package(PkgConfig)

pkg_check_modules(grp grp>=0.4.21)
