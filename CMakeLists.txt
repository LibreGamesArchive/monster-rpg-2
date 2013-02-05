project(monster-rpg-2)
cmake_minimum_required(VERSION 2.6)

OPTION(KCM_AUDIO "Use Allegro audio instead of BASS" off)
OPTION(WITH_SVG "Enable SVG loader" off)

if(KCM_AUDIO)
	set(CMAKE_C_FLAGS "-DKCM_AUDIO")
	set(CMAKE_CXX_FLAGS "-DKCM_AUDIO")
endif(KCM_AUDIO)

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

if(WIN32)
	if(MINGW)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -fcheck-new -Wno-unknown-pragmas -Wno-comment -Wno-strict-aliasing")
		# you must define one of A5_OGL (all platforms) or A5_D3D
		#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DA5_OGL")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DA5_D3D")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
	else(MINGW)
		#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DA5_OGL")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DA5_D3D")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
	endif(MINGW)
else(WIN32)
	if(RASPBERRYPI)
		set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -DA5_OGL -march=armv6 -mfpu=vfp -mfloat-abi=hard")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DA5_OGL -march=armv6 -mfpu=vfp -mfloat-abi=hard")
	else(RASPBERRYPI)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DA5_OGL")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DA5_OGL")
	endif(RASPBERRYPI)
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
	set(COMMON_SRCS ${COMMON_SRCS} src/init.mm src/joypad.mm src/joypad_handler.m src/gamecenter.mm)
else(APPLE)
	set(COMMON_SRCS ${COMMON_SRCS} src/init.cpp)
endif(APPLE)
endif(WIN32)


set(MAIN_LIBS "")

if(WITH_SVG)
	list(APPEND MAIN_LIBS "allegro_color${ALLEG_SUFFIX}" "wapcaplet" "svgtiny" "dom" "expat" "hubbub" "parserutils")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWITH_SVG -I/opt/netsurf/include")
	set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -DWITH_SVG -I/opt/netsurf/include")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/netsurf/lib")
	set(COMMON_SRCS ${COMMON_SRCS} src/svg.cpp)
endif()

add_library(mo2-engine
	STATIC
	${COMMON_SRCS}
)

add_executable(monster2
	src/monster2.cpp
)

list(APPEND MAIN_LIBS "mo2-engine")

if(KCM_AUDIO)
	if(RASPBERRYPI)
		LIST(APPEND MAIN_LIBS "allegro_audio${ALLEG_SUFFIX}" "allegro_acodec${ALLEG_SUFFIX}" "/usr/lib/arm-linux-gnueabihf/libFLAC.a" "/usr/lib/arm-linux-gnueabihf/libvorbisfile.a" "/usr/lib/arm-linux-gnueabihf/libvorbis.a" "/usr/lib/arm-linux-gnueabihf/libogg.a")
	else()
		LIST(APPEND MAIN_LIBS "allegro_audio${ALLEG_SUFFIX}" "allegro_acodec${ALLEG_SUFFIX}" "FLAC" "vorbisfile" "vorbis" "ogg")
	endif()
endif(KCM_AUDIO)


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
		"z"
	)
	LIST(APPEND MAIN_LIBS 
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
		lua5.2
		d3dx9
	)
	if(KCM_AUDIO)
		LIST(APPEND MAIN_LIBS "ws2_32" "dsound")
	else(KCM_AUDIO)
		LIST(APPEND MAIN_LIBS "bass")
	endif(KCM_AUDIO)
else(WIN32)
if(APPLE)
	set(ALLEGRO_MAIN_LIB "allegro_main${ALLEG_SUFFIX}")
	FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
	FIND_LIBRARY(FOUNDATION_LIBRARY Foundation)
	FIND_LIBRARY(APPSERVICES_LIBRARY ApplicationServices)
	FIND_LIBRARY(APPKIT_LIBRARY AppKit)
	FIND_LIBRARY(IOKIT_LIBRARY IOKit)
	FIND_LIBRARY(AUDIOTOOLBOX_LIBRARY AudioToolbox)
	FIND_LIBRARY(GAMEKIT_LIBRARY GameKit)
	MARK_AS_ADVANCED(OPENGL_LIBRARY)
	MARK_AS_ADVANCED(FOUNDATION_LIBRARY)
	MARK_AS_ADVANCED(APPSERVICES_LIBRARY)
	MARK_AS_ADVANCED(IOKIT_LIBRARY)
	SET(EXTRA_LIBS ${IOKIT_LIBRARY} ${OPENGL_LIBRARY} ${FOUNDATION_LIBRARY} ${APPSERVICES_LIBRARY} ${APPKIT_LIBRARY} ${AUDIOTOOLBOX_LIBRARY} ${GAMEKIT_LIBRARY} freetype z)
	LIST(APPEND MAIN_LIBS 
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
		lua5.2
		JoypadCocoa
		curl
	)

	if(KCM_AUDIO)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework OpenAL")
	else(KCM_AUDIO)
		LIST(APPEND MAIN_LIBS "bass")
	endif(KCM_AUDIO)
else(APPLE)
	if(NOT KCM_AUDIO)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,.")
	endif(NOT KCM_AUDIO)
	LIST(APPEND MAIN_LIBS 
		mo2-engine
		allegro_ttf${ALLEG_SUFFIX}
		allegro_font${ALLEG_SUFFIX}
		allegro_primitives${ALLEG_SUFFIX}
		allegro_memfile${ALLEG_SUFFIX}
		allegro_image${ALLEG_SUFFIX}
		allegro_shader${ALLEG_SUFFIX}
		allegro${ALLEG_SUFFIX}
		${ALLEGRO_MAIN_LIB}
		lua5.2
		curl
		m pthread SM ICE X11 Xext Xcursor Xinerama Xrandr Xpm
	)
        if(KCM_AUDIO)
		if(RASPBERRYPI)
			LIST(APPEND MAIN_LIBS "asound")
		else()
			LIST(APPEND MAIN_LIBS "pulse-simple" "pulse" "asound" "openal")
		endif()
	else(KCM_AUDIO)
		LIST(APPEND MAIN_LIBS "bass")
	endif(KCM_AUDIO)
	if(RASPBERRYPI)
		LIST(APPEND MAIN_LIBS GLESv2 EGL bcm_host /usr/lib/arm-linux-gnueabihf/libfreetype.a /usr/lib/arm-linux-gnueabihf/libz.a /usr/lib/arm-linux-gnueabihf/libpng.a /usr/lib/arm-linux-gnueabihf/libjpeg.a)
	else(RASPBERYPI)
		LIST(APPEND MAIN_LIBS allegro_dialog${ALLEG_SUFFIX} GL GLU freetype z png jpeg)
	endif(RASPBERRYPI)
endif(APPLE)
endif(WIN32)

target_link_libraries(
	monster2
	${MAIN_LIBS}
) 

