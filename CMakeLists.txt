project(monster-rpg-2)
cmake_minimum_required(VERSION 2.6)

# function to copy files to build dir:
function(copy_file_to_build target file dest)
   add_custom_target(${target} ALL DEPENDS ${dest})

   add_custom_command(
      OUTPUT ${dest}
      COMMAND "${CMAKE_COMMAND}" -E copy
         "${CMAKE_CURRENT_SOURCE_DIR}/${file}" ${dest}
   )

endfunction(copy_file_to_build)

# Option configuration

set(ALLEG_SUFFIX "-static")
	
if(WIN32)
	if(MINGW)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -fcheck-new -Wno-unknown-pragmas -Wno-comment -Wno-strict-aliasing")
		# you must define one of A5_OGL (all platforms) or A5_D3D
		#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DA5_OGL")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DA5_D3D")
	else(MINGW)
		#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DA5_OGL")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DA5_D3D")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
	endif(MINGW)
else(WIN32)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DA5_OGL")
endif(WIN32)

include_directories("${CMAKE_SOURCE_DIR}/include")

if(NOT ("${USER_INCLUDE_PATH}" STREQUAL ""))
	include_directories(${USER_INCLUDE_PATH})
endif(NOT ("${USER_INCLUDE_PATH}" STREQUAL ""))
if(NOT ("${USER_LIBRARY_PATH}" STREQUAL ""))
	link_directories(${USER_LIBRARY_PATH})
endif(NOT ("${USER_LIBRARY_PATH}" STREQUAL ""))

set(COMMON_SRCS
	src/3d.cpp
	src/Animation.cpp
	src/AnimationSet.cpp
	src/Area.cpp
	src/CombatActions.cpp
	src/CombatEnemy.cpp
	src/CombatEntity.cpp
	src/CombatPlayer.cpp
	src/Combatant.cpp
	src/Configuration.cpp
	src/Frame.cpp
	src/GenericEffect.cpp
	src/Image.cpp
	src/Input.cpp
	src/Items.cpp
	src/Object.cpp
	src/Player.cpp
	src/Spells.cpp
	src/Tile.cpp
	src/battle.cpp
	src/debug.cpp
	src/equipment.cpp
	src/ftpget.cpp
	src/graphics.cpp
	src/inventory.cpp
	src/io.cpp
	src/lander.cpp
	src/pause.cpp
	src/redundant.cpp
	src/script.cpp
	src/shooter.cpp
	src/sound.cpp
	src/tftp_get.c
	src/tgui.cpp
	src/translate.cpp
	src/util.cpp
	src/widgets.cpp
	src/xml.cpp
)

if(WIN32)
	set(COMMON_SRCS ${COMMON_SRCS} src/init.cpp)
else(WIN32)
if(APPLE)
	set(COMMON_SRCS ${COMMON_SRCS} src/init.mm src/joypad.mm src/joypad_handler.m)
else(APPLE)
	set(COMMON_SRCS ${COMMON_SRCS} src/init.cpp)
endif(APPLE)
endif(WIN32)

add_library(mo2-engine
	STATIC
	${COMMON_SRCS}
)

add_executable(monster2
	src/monster2.cpp
)

if(WIN32)
	set(ALLEGRO_MAIN_LIB "")
	set(EXTRA_LIBS
		"curl"
		"opengl32"
		"dxguid"
		"dinput8"
		"dsound"
		"d3d9"
		"kernel32"
		"user32"
		"gdi32"
		"comdlg32"
		"ole32"
		"winmm"
		"psapi"
		"gdiplus"
		"uuid"
		"shlwapi"
		"wsock32"
		"ws2_32"
		"freetype"
		"bass"
		"z"
	)
	set(MAIN_LIBS 
		mo2-engine
		allegro_ttf${ALLEG_SUFFIX}
		allegro_font${ALLEG_SUFFIX}
		allegro_primitives${ALLEG_SUFFIX}
		allegro_dialog${ALLEG_SUFFIX}
		allegro_memfile${ALLEG_SUFFIX}
		allegro_image${ALLEG_SUFFIX}
		allegro_shader${ALLEG_SUFFIX}
		allegro${ALLEG_SUFFIX}
		${ALLEGRO_MAIN_LIB}
		${EXTRA_LIBS}
		lua-fixed
		d3dx9
	)
else(WIN32)
if(APPLE)
	set(ALLEGRO_MAIN_LIB "allegro_main${ALLEG_SUFFIX}")
	FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
	FIND_LIBRARY(FOUNDATION_LIBRARY Foundation)
	FIND_LIBRARY(APPSERVICES_LIBRARY ApplicationServices)
	FIND_LIBRARY(APPKIT_LIBRARY AppKit)
	FIND_LIBRARY(IOKIT_LIBRARY IOKit)
	FIND_LIBRARY(AUDIOTOOLBOX_LIBRARY AudioToolbox)
	MARK_AS_ADVANCED(OPENGL_LIBRARY)
	MARK_AS_ADVANCED(FOUNDATION_LIBRARY)
	MARK_AS_ADVANCED(APPSERVICES_LIBRARY)
	MARK_AS_ADVANCED(IOKIT_LIBRARY)
	SET(EXTRA_LIBS ${IOKIT_LIBRARY} ${OPENGL_LIBRARY} ${FOUNDATION_LIBRARY} ${APPSERVICES_LIBRARY} ${APPKIT_LIBRARY} ${AUDIOTOOLBOX_LIBRARY} freetype bass z)
	set(MAIN_LIBS
		mo2-engine
		allegro_ttf${ALLEG_SUFFIX}
		allegro_font${ALLEG_SUFFIX}
		allegro_primitives${ALLEG_SUFFIX}
		allegro_dialog${ALLEG_SUFFIX}
		allegro_memfile${ALLEG_SUFFIX}
		allegro_image${ALLEG_SUFFIX}
		allegro_shader${ALLEG_SUFFIX}
		allegro${ALLEG_SUFFIX} ${ALLEGRO_MAIN_LIB}
		${EXTRA_LIBS}
		lua-fixed
		JoypadCocoa
		curl
	)
else(APPLE)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,.")
	set(MAIN_LIBS
		mo2-engine
		allegro_ttf${ALLEG_SUFFIX}
		allegro_font${ALLEG_SUFFIX}
		allegro_primitives${ALLEG_SUFFIX}
		allegro_dialog${ALLEG_SUFFIX}
		allegro_memfile${ALLEG_SUFFIX}
		allegro_image${ALLEG_SUFFIX}
		allegro_shader${ALLEG_SUFFIX}
		allegro${ALLEG_SUFFIX}
		${ALLEGRO_MAIN_LIB}
		lua-fixed
		m pthread SM ICE X11 Xext Xcursor Xinerama Xrandr Xpm GL GLU
		freetype
		z
		bass
		png jpeg curl
	) 
endif(APPLE)
endif(WIN32)

target_link_libraries(
	monster2
	${MAIN_LIBS}
) 

