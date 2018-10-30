cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project("StarClone" C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

# place binaries and libraries according to GNU standards
#include(GNUInstallDirs)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# example how to set c++ compiler flags for GNU
set(ADDVARS "")

# project version
set(VERSION_MAJOR 0)
set(VERSION_MINOR 3)
set(VERSION_PATCH 3)


include_directories(include)
include_directories(include/network)
include_directories(include/smacker)

set(SOURCES
	src/MenuItem.cpp
	src/action.cpp
	src/actiondefault.cpp
	src/audio.cpp
	src/auxil.cpp
	src/briefing.cpp
	src/commandqueue.cpp
	src/construct.cpp
	src/creep.cpp
	src/debug.cpp
	src/dialogbin.cpp
	src/diap.cpp
	src/doodad.cpp
	src/flingy.cpp
	src/fog.cpp
	src/fonts.cpp
	src/gamemenu.cpp
	src/gener.cpp
	src/gr.cpp
	src/hod.cpp
	src/images.cpp
	src/iscript.cpp
	src/key.cpp
	src/lists.cpp
	src/load.cpp
	src/loadlo.cpp
	src/mage.cpp
	src/man.cpp
	src/maps.cpp
	src/market.cpp
	src/menu.cpp
	src/messages.cpp
	src/mouse.cpp
	src/mousenew.cpp
	src/mpq.cpp
	src/mpqwrapper.cpp
	src/music.cpp
	src/mycycle.cpp
	src/myfifo.cpp
	src/mylist.cpp
	src/mytime.cpp
	src/objinfo.cpp
	src/objlists.cpp
	src/objs.cpp
	src/overlay.cpp
	src/pcx.cpp
	src/player.cpp
	src/portrait.cpp
	src/putobj.cpp
	src/pylon.cpp
	src/queue.cpp
	src/rand.cpp
	src/rules.cpp
	src/scripts.cpp
	src/scrolltile.cpp
	src/sigorder.cpp
	src/starmap.cpp
	src/stars.cpp
	src/stringfiles.cpp
	src/tbl.cpp
	src/tempobj.cpp
	src/triggers.cpp
	src/unitaction.cpp
	src/upgrade.cpp
	src/vars.cpp
	src/video.cpp
	src/vision.cpp
	src/walk.cpp
	src/weapons.cpp
	src/wmem.cpp
	src/network/netwgame.cpp
	src/network/network_dc.cpp
	src/network/network_dedicated.cpp
	src/network/network_general.cpp
	src/network/network_modem.cpp
	src/network/network_tcp.cpp
	src/network/network_udp.cpp
	src/network/netwplay.cpp
	src/smacker/smacker.cpp
	src/smacker/smk_bitstream.cpp
	src/smacker/smk_hufftree.cpp
    )

find_package(SDL 1.2.5 REQUIRED)

if(SDL_FOUND)
    set(ADDVARS "${ADDVARS} -DWITHSDL")
    include_directories(include/sdl)
    list(APPEND SOURCES src/sdl/grsdl.cpp src/sdl/mousesdl.cpp)
endif()

find_package(SDL_mixer 1.2.5)

if(SDL_mixer_FOUND)
    set(ADDVARS "${ADDVARS} -DWITHSDLMIXER")
    list(APPEND SOURCES src/sdl/grsdl.cpp src/sound.sdl/audiolowlevel.cpp)
else()
    list(APPEND SOURCES src/sdl/grsdl.cpp src/sound.null/audiolowlevel.cpp)
endif()

find_package(grp REQUIRED)
find_package(mpq)
#ind_package(StormLib QUIET)
find_package(ZLIB 1.0.2 REQUIRED)

if(!mpq_FOUND)
    if(!StormLib_FOUND)
	message("mpqlib or StormLib not found")
    else()
	set(ADDVARS "${ADDVARS} -DSTORMLIB")
	link_libraries(${StormLib_LIBRARIES})
    endif()
else()
    set(ADDVARS "${ADDVARS} -DMPQLIB")
    link_libraries(${mpq_LIBRARIES})
endif()


if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(ADDVARS "${ADDVARS} -DUNDERLINUX")
    set(WARN "-Wno-unknown-pragmas -Wno-sign-compare -Woverloaded-virtual -Wwrite-strings -Wno-unused")
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} ${WARN} ${ADDVARS}")
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

link_libraries(${grp_LIBRARIES})
link_libraries(${SDL_LIBRARY})
link_libraries(${SDL_MIXER_LIBRARIES})
link_libraries(${ZLIB_LIBRARIES})

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)
add_executable(StarClone ${SOURCES} )
