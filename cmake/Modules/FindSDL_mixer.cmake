# - Try to find SDL_mixer
# Once done, this will define
#
#  SDL_mixer_FOUND - system has SDL_mixerlib
#  SDL_mixer_INCLUDE_DIRS - the SDL_mixerlib include directories
#  SDL_mixer_LIBRARIES - link these to use SDL_mixerlib

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(SDL_mixer_PKGCONF SDL_mixer)

# Main include dir
find_path(SDL_mixer_INCLUDE_DIR
  NAMES SDL_mixer.h
  PATHS ${SDL_mixer_PKGCONF_INCLUDE_DIRS}
  PATH_SUFFIXES SDL_mixer
)

# Finally the library itself
find_library(SDL_mixer_LIBRARY
  NAMES SDL_mixer
  PATHS ${SDL_mixer_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(SDL_mixer_PROCESS_INCLUDES SDL_mixer_INCLUDE_DIR SDL_mixerConfig_INCLUDE_DIR)
set(SDL_mixer_PROCESS_LIBS SDL_mixer_LIBRARY)
libfind_process(SDL_mixer)
