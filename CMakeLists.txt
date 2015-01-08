project(monster-rpg-2)
cmake_minimum_required(VERSION 2.6)

OPTION(KCM_AUDIO "Use Allegro audio instead of BASS" off)
OPTION(WITH_SVG "Enable SVG loader" off)
OPTION(RASPBERRYPI "Compile for Raspberry Pi" off)
OPTION(DEMO "Build demo version" off)

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
	set(M2_SUFFIX "")
	if(MINGW)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -fcheck-new -Wno-unknown-pragmas -Wno-comment -Wno-strict-aliasing")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
	else(MINGW)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D_USE_MATH_DEFINES /DALLEGRO_STATICLINK /EHsc /D_WIN32_WINNT=0x501")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_USE_MATH_DEFINES /DALLEGRO_STATICLINK /EHsc /D_WIN32_WINNT=0x501")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO /SUBSYSTEM:WINDOWS")
	endif(MINGW)
else(WIN32)
	if(RASPBERRYPI)
		set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Os -march=armv6 -mfpu=vfp -mfloat-abi=hard")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -march=armv6 -mfpu=vfp -mfloat-abi=hard")
   		include_directories(
			"/opt/vc/include"
			"/opt/vc/include/interface/vcos/pthreads"
			"/opt/vc/include/interface/vmcs_host/linux"
		)
		link_directories(
			"/opt/vc/lib"
		)
	else(RASPBERRYPI)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
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
	src/atlas.cpp
	src/atlas_accessor.cpp
	src/battle.cpp
	src/debug.cpp
	src/equipment.cpp
	src/graphics.cpp
	src/inventory.cpp
	src/io.cpp
	src/lander.cpp
	src/my_load_bitmap.cpp
	src/pause.cpp
	src/redundant.cpp
	src/script.cpp
	src/shooter.cpp
	src/sound.cpp
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
	set(COMMON_SRCS ${COMMON_SRCS} src/init.mm src/joypad.mm src/joypad_handler.mm src/gamecenter.mm src/mygamecentervc.m)
else(APPLE)
	set(COMMON_SRCS ${COMMON_SRCS} src/init.cpp)
endif(APPLE)
endif(WIN32)


set(EXTRA_LIBS "")

if(WITH_SVG)
	list(APPEND EXTRA_LIBS "allegro_color${ALLEG_SUFFIX}" "svgtiny" "hubbub" "dom" "parserutils" "wapcaplet" "expat")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWITH_SVG -I/opt/netsurf/include")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_SVG -I/opt/netsurf/include")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/netsurf/lib")
	set(COMMON_SRCS ${COMMON_SRCS} src/svg.cpp)
endif()

if(DEMO)
	if(WIN32)
		if(MINGW)
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEMO=1")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEMO=1")
		else()
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /DDEMO=1")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DDEMO=1")
		endif()
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEMO=1")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEMO=1")
	endif()
endif()

add_library(mo2-engine
	STATIC
	${COMMON_SRCS}
)

add_executable(monster2${M2_SUFFIX}
	src/monster2.cpp
)

if(KCM_AUDIO)
	if(RASPBERRYPI)
		LIST(APPEND EXTRA_LIBS "allegro_audio${ALLEG_SUFFIX}" "allegro_acodec${ALLEG_SUFFIX}" "/usr/lib/arm-linux-gnueabihf/libFLAC.a" "/usr/lib/arm-linux-gnueabihf/libvorbisfile.a" "/usr/lib/arm-linux-gnueabihf/libvorbis.a" "/usr/lib/arm-linux-gnueabihf/libogg.a")
	else()
		LIST(APPEND EXTRA_LIBS "allegro_audio${ALLEG_SUFFIX}" "allegro_acodec${ALLEG_SUFFIX}" "FLAC" "vorbisfile" "vorbis" "ogg")
	endif()
endif(KCM_AUDIO)


if(WIN32)
	set(ALLEGRO_MAIN_LIB "")
	list(APPEND EXTRA_LIBS
		"curl"
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
	if (MINGW)
		set(ICON_OBJ ${CMAKE_SOURCE_DIR}/src/icon.res)
	else()
		set(ICON_OBJ ${CMAKE_SOURCE_DIR}/src/icon.lib)
	endif()
	SET(MAIN_LIBS 
		${ICON_OBJ}
		mo2-engine
		allegro_ttf${ALLEG_SUFFIX}
		allegro_font${ALLEG_SUFFIX}
		allegro_primitives${ALLEG_SUFFIX}
		allegro_dialog${ALLEG_SUFFIX}
		allegro_memfile${ALLEG_SUFFIX}
		allegro_image${ALLEG_SUFFIX}
		allegro${ALLEG_SUFFIX}
		${ALLEGRO_MAIN_LIB}
		${EXTRA_LIBS}
		lua5.2
		d3dx9
		"opengl32"
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
	FIND_LIBRARY(SECURITY_LIBRARY Security)
	LIST(APPEND EXTRA_LIBS ${IOKIT_LIBRARY} ${OPENGL_LIBRARY} ${FOUNDATION_LIBRARY} ${APPSERVICES_LIBRARY} ${APPKIT_LIBRARY} ${AUDIOTOOLBOX_LIBRARY} ${GAMEKIT_LIBRARY} ${SECURITY_LIBRARY} freetype z)
	SET(MAIN_LIBS 
		mo2-engine
		allegro_ttf${ALLEG_SUFFIX}
		allegro_font${ALLEG_SUFFIX}
		allegro_primitives${ALLEG_SUFFIX}
		allegro_dialog${ALLEG_SUFFIX}
		allegro_memfile${ALLEG_SUFFIX}
		allegro_image${ALLEG_SUFFIX}
		allegro${ALLEG_SUFFIX} ${ALLEGRO_MAIN_LIB}
		${EXTRA_LIBS}
		lua5.2
		curl
		png
		jpeg
	)
	if(KCM_AUDIO)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework OpenAL")
	else(KCM_AUDIO)
		LIST(APPEND MAIN_LIBS "bass")
	endif(KCM_AUDIO)
else(APPLE)
	if(NOT KCM_AUDIO)
		# Ubuntu software center build:
		#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,/opt/monster-rpg-2")
		# Normal build:
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,.")
	endif(NOT KCM_AUDIO)
	SET(MAIN_LIBS 
		mo2-engine
		allegro_ttf${ALLEG_SUFFIX}
		allegro_font${ALLEG_SUFFIX}
		allegro_primitives${ALLEG_SUFFIX}
		allegro_memfile${ALLEG_SUFFIX}
		allegro_image${ALLEG_SUFFIX}
		allegro_color${ALLEG_SUFFIX}
		allegro${ALLEG_SUFFIX}
		${ALLEGRO_MAIN_LIB}
		${EXTRA_LIBS}
		m pthread SM ICE X11 Xext Xcursor Xinerama Xrandr Xpm
	)
        if(KCM_AUDIO)
		if(RASPBERRYPI)
			LIST(APPEND MAIN_LIBS "asound")
		else()
			LIST(APPEND MAIN_LIBS "pulse-simple" "pulse" "asound" "openal")
		endif()
	else(KCM_AUDIO)
		LIST(APPEND MAIN_LIBS "${CMAKE_BINARY_DIR}/libbass.so")
	endif(KCM_AUDIO)
	if(RASPBERRYPI)
		LIST(APPEND MAIN_LIBS GLESv2 EGL bcm_host /usr/lib/arm-linux-gnueabihf/libfreetype.a /usr/lib/arm-linux-gnueabihf/libz.a /usr/lib/arm-linux-gnueabihf/libpng.a /usr/lib/arm-linux-gnueabihf/liblua5.2.a /usr/local/lib/libcurl.a)
	else(RASPBERYPI)
		# trentg's build
		LIST(APPEND MAIN_LIBS allegro_dialog${ALLEG_SUFFIX} GL GLU /usr/lib/libfreetype.a /usr/lib/libz.a /usr/lib/libpng.a /usr/lib/libjpeg.a /usr/local/lib/liblua5.2.a /usr/local/lib/libcurl.a)
		# normal build
		#LIST(APPEND MAIN_LIBS allegro_dialog${ALLEG_SUFFIX} GL GLU freetype z png jpeg lua5.2 curl)
	endif(RASPBERRYPI)
endif(APPLE)
endif(WIN32)

target_link_libraries(
	monster2${M2_SUFFIX}
	${MAIN_LIBS}
) 

