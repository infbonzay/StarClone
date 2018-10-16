# - Try to find SDL
# Once done, this will define
#
#  SDL_FOUND - system has SDLlib
#  SDL_INCLUDE_DIRS - the SDLlib include directories
#  SDL_LIBRARIES - link these to use SDLlib

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(SDL_PKGCONF SDL)

# Main include dir
find_path(SDL_INCLUDE_DIR
  NAMES SDL.h
  PATHS ${SDL_PKGCONF_INCLUDE_DIRS}
  PATH_SUFFIXES SDL
)

# Finally the library itself
find_library(SDL_LIBRARY
  NAMES SDL
  PATHS ${SDL_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(SDL_PROCESS_INCLUDES SDL_INCLUDE_DIR SDLConfig_INCLUDE_DIR)
set(SDL_PROCESS_LIBS SDL_LIBRARY)
libfind_process(SDL)
