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

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)

set(SOURCES
	src/LeaderBoard.cpp
	src/RegenObjMap.cpp
	src/ScreenDraw.cpp
	src/MenuItem.cpp
	src/gener.cpp
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
	src/mpq.cpp
	src/mpqwrapper.cpp
	src/music.cpp
	src/mylist.cpp
	src/mytime.cpp
	src/ObjChecks.cpp
	src/objlists.cpp
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
	src/objs.cpp
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

include_directories(include/video)

if (UNIX)

	add_definitions(-DUNDERLINUX)
	add_definitions(-DWITHNETUDP)

	find_package(SDL 1.2.5)
	if(SDL_FOUND)
		message("-- Using SDL video library")
		add_definitions(-DWITHSDL)
		list(APPEND SOURCES src/video/sdl/Controller.cpp)
		link_libraries(${SDL_LIBRARY})
	else()
		message("-- Using xlib video library")
		find_package(X11 REQUIRED)
		if(X11_FOUND)
			if(X11_xf86vmode_FOUND)
				add_definitions(-DWITHXEXT)
				link_libraries(${X11_Xxf86vm_LIB})
			else()
				message("No X11 Extension (xxf86vm) found")
				return()
			endif()
			add_definitions(-DWITHX)
			include_directories(${X11_INCLUDE_DIR})
			list(APPEND SOURCES src/video/xlib/Controller.cpp)
			link_libraries(${X11_LIBRARIES})
		else()
			#set(FB_FOUND /have)
			if(FB_FOUND)
				message("-- Using Framebuffer video library")
				add_definitions(-DWITHFB)
				list(APPEND SOURCES src/video/fb/Controller.cpp)
				#link_libraries(${SDL_LIBRARY})
			else()
				message("No X server found")
				return()
			endif()
		endif()
	endif()
	if(SDL_FOUND)
		find_package(SDL_mixer 1.2.5)
	endif()
	if(SDL_mixer_FOUND)
		add_definitions(-DWITHSDLAUDIO)
		list(APPEND SOURCES src/audio/sdl/audiolib.cpp)
		link_libraries(${SDL_MIXER_LIBRARIES})
	else()
		add_definitions(-DWITHNOAUDIO)
		list(APPEND SOURCES src/audio/none/audiolib.cpp)
	endif()

	find_package(grp)
	if(grp_FOUND)
		link_libraries(${grp_LIBRARIES})
	else()
		message("--   grplib not found, use internal grplib")
		set(INCLUDE_GRP /have)
	endif()
	
	find_package(StormLib)
	if(StormLib_FOUND)
		add_definitions(-DWITHSTORMLIB)
		link_libraries(${StormLib_LIBRARIES})
	else()
		add_definitions(-DWITHMPQLIB)
		find_package(mpq)
		if(mpq_FOUND)
			link_libraries(${mpq_LIBRARIES})
		else()
			message("--   mpqlib,StormLib not found, use internal mpqlib")
			set(INCLUDE_MPQ /have)
		endif()
		
	endif()

	find_package(ZLIB 1.0.2 REQUIRED)

	set(WARN "-Wno-unknown-pragmas -Wno-sign-compare -Woverloaded-virtual -Wwrite-strings -Wno-unused")
	set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} ${WARN}")
	set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3 -DDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")

	link_libraries(${ZLIB_LIBRARIES})
else()
	if (WIN32)
		message("-- Using Windows")
		add_definitions(-DUNDERWINDOWS)
		add_definitions(-DWITHNOAUDIO)
		list(APPEND SOURCES src/audio/none/audiolib.cpp)
		include_directories(..)
		include_directories(../zlib)
		include_directories(include/win)
		include_directories( include/libs/grplib)
		list(APPEND SOURCES src/video/win/Controller.cpp)
		set(INCLUDE_GRP /have)
		set(INCLUDE_MPQ /have)
	else()
		message("Unknown target system")
		return()
	endif()
endif()

if(INCLUDE_GRP)
	add_definitions(-DVERSION="0.4.20")
	include_directories( include/libs/grplib)
	list(APPEND SOURCES src/libs/grplib/src/gr8.cpp)
	list(APPEND SOURCES src/libs/grplib/src/grp.cpp)
	list(APPEND SOURCES src/libs/grplib/src/palette.cpp)
	list(APPEND SOURCES src/libs/grplib/src/usegrp.cpp)
	list(APPEND SOURCES src/libs/grplib/src/cpp/font.cpp)
	list(APPEND SOURCES src/libs/grplib/src/cpp/grpcpp.cpp)
endif()
if(INCLUDE_MPQ)
	include_directories( include/libs/mpqlib)
	include_directories( include/libs/mpqlib/mpqlib)
	include_directories( src/libs/mpqlib/)
	list(APPEND SOURCES src/libs/mpqlib/huffman/huffman.cpp)
	list(APPEND SOURCES src/libs/mpqlib/pklib/crc32.cpp)
	list(APPEND SOURCES src/libs/mpqlib/pklib/explode.cpp)
	list(APPEND SOURCES src/libs/mpqlib/pklib/implode.cpp)
	list(APPEND SOURCES src/libs/mpqlib/wave/wave.cpp)
	list(APPEND SOURCES src/libs/mpqlib/src/SCommon.cpp)
	list(APPEND SOURCES src/libs/mpqlib/src/SCompression.cpp)
	list(APPEND SOURCES src/libs/mpqlib/src/SFileCompactArchive.cpp)
	list(APPEND SOURCES src/libs/mpqlib/src/SFileCreateArchiveEx.cpp)
	list(APPEND SOURCES src/libs/mpqlib/src/SFileExtractFile.cpp)
	list(APPEND SOURCES src/libs/mpqlib/src/SFileFindFile.cpp)
	list(APPEND SOURCES src/libs/mpqlib/src/SFileOpenArchive.cpp)
	list(APPEND SOURCES src/libs/mpqlib/src/SFileOpenFileEx.cpp)
	list(APPEND SOURCES src/libs/mpqlib/src/SFileReadFile.cpp)
	list(APPEND SOURCES src/libs/mpqlib/src/SListFile.cpp)
	list(APPEND SOURCES src/libs/mpqlib/src/StormPortLinux.cpp)
endif()

add_executable(StarClone ${SOURCES} )
if(WIN32)
	target_link_libraries(StarClone wsock32 ws2_32)
	target_link_libraries(StarClone ../../zlib/zlib)
endif()
