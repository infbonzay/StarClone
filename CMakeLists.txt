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
	src/objinfo.cpp
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

if (UNIX)

	add_definitions(-DUNDERLINUX)
	add_definitions(-DWITHNETUDP)
	include_directories(include/video)

	#set(FB_FOUND /have)

	if(FB_FOUND)
		message("-- Using Framebuffer video library")
		add_definitions(-DWITHFB)
		list(APPEND SOURCES src/video/fb/Controller.cpp)
		#link_libraries(${SDL_LIBRARY})
	else()
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
		add_definitions(-DVERSION="0.4.20")
		include_directories( include/libs/grplib)
		list(APPEND SOURCES src/libs/grplib/src/gr8.cpp)
		list(APPEND SOURCES src/libs/grplib/src/grp.cpp)
		list(APPEND SOURCES src/libs/grplib/src/palette.cpp)
		list(APPEND SOURCES src/libs/grplib/src/usegrp.cpp)
		list(APPEND SOURCES src/libs/grplib/src/cpp/font.cpp)
		list(APPEND SOURCES src/libs/grplib/src/cpp/grpcpp.cpp)
		set_source_files_properties(src/libs/grplib/src/gr8.cpp PROPERTIES COMPILE_FLAGS -funsigned-char)
		set_source_files_properties(src/libs/grplib/src/grp.cpp PROPERTIES COMPILE_FLAGS -funsigned-char)
		set_source_files_properties(src/libs/grplib/src/palette.cpp PROPERTIES COMPILE_FLAGS -funsigned-char)
		set_source_files_properties(src/libs/grplib/src/usegrp.cpp PROPERTIES COMPILE_FLAGS -funsigned-char)
		set_source_files_properties(src/libs/grplib/src/cpp/font.cpp PROPERTIES COMPILE_FLAGS -funsigned-char)
		set_source_files_properties(src/libs/grplib/src/cpp/grpcpp.cpp PROPERTIES COMPILE_FLAGS -funsigned-char)
	endif()
	
	find_package(StormLib)
	if(StormLib_FOUND)
		add_definitions(-DSTORMLIB)
		link_libraries(${StormLib_LIBRARIES})
	else()
		add_definitions(-DMPQLIB)
		find_package(mpq)
		if(mpq_FOUND)
			include_directories( include/libs/mpqlib/include)
		else()
			link_libraries(${mpq_LIBRARIES})
		endif()
		
	endif()

	find_package(mpq)
	if(NOT mpq_FOUND)
		find_package(StormLib)
		if(NOT StormLib_FOUND)
			add_definitions(-DMPQLIB)
			include_directories( include/libs/mpqlib)
			#message("mpqlib or StormLib not found")
		else()
			add_definitions(-DSTORMLIB)
			link_libraries(${StormLib_LIBRARIES})
		endif()
	else()
		add_definitions(-DMPQLIB)
		link_libraries(${mpq_LIBRARIES})
	endif()
	find_package(ZLIB 1.0.2 REQUIRED)

	set(WARN "-Wno-unknown-pragmas -Wno-sign-compare -Woverloaded-virtual -Wwrite-strings -Wno-unused")
	set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} ${WARN}")
	set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3 -DDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")

	link_libraries(${ZLIB_LIBRARIES})
else()
	add_compile_definitions(UNDERWINDOWS)
	add_compile_definitions(WITHSDL)
	#add_compile_definitions(WITHSDLMIXER)
	add_compile_definitions(MPQLIB)
	include_directories(..)
	include_directories(include/win)

endif()

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/)
add_executable(StarClone ${SOURCES} )

