project(monster-rpg-2)
cmake_minimum_required(VERSION 2.6)

OPTION(KCM_AUDIO "Use Allegro audio instead of BASS" off)
OPTION(WITH_SVG "Enable SVG loader" off)
OPTION(RASPBERRYPI "Compile for Raspberry Pi" off)
OPTION(DEMO "Build demo version" off)
OPTION(STEAMWORKS "Enable Steamworks" off)
OPTION(STOCK_ALLEGRO "Use stock Allegro libraries" off)

set(ALLEGRO_SUFFIX "-static" CACHE STRING "Allegro library suffix")

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

if(STOCK_ALLEGRO)
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DSTOCK_ALLEGRO")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSTOCK_ALLEGRO")
endif()

if(WIN32)
	set(M2_SUFFIX "")
else(WIN32)
	if(RASPBERRYPI)
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -march=armv6 -mfpu=vfp -mfloat-abi=hard")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -march=armv6 -mfpu=vfp -mfloat-abi=hard")
   		include_directories(
			"/opt/vc/include"
			"/opt/vc/include/interface/vcos/pthreads"
			"/opt/vc/include/interface/vmcs_host/linux"
		)
		link_directories(
			"/opt/vc/lib"
		)
	else()
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
	endif()
	if(STEAMWORKS)
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DSTEAMWORKS")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSTEAMWORKS")
	endif()
endif()

include_directories("${CMAKE_SOURCE_DIR}/include")

if(NOT ("${USER_INCLUDE_PATH}" STREQUAL ""))
	include_directories(${USER_INCLUDE_PATH})
endif()
if(NOT ("${USER_LIBRARY_PATH}" STREQUAL ""))
	link_directories(${USER_LIBRARY_PATH})
endif()

set(COMMON_SOURCES
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

if(APPLE)
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNO_GAMECENTER")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNO_GAMECENTER")
	set(COMMON_SOURCES ${COMMON_SOURCES} src/init.mm src/gamecenter.mm)
else()
	set(COMMON_SOURCES ${COMMON_SOURCES} src/init.cpp)
endif()

set(EXTRA_LIBS "")

if(WITH_SVG)
	list(APPEND EXTRA_LIBS "allegro_color${ALLEGRO_SUFFIX}" "svgtiny" "hubbub" "dom" "parserutils" "wapcaplet" "expat")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DWITH_SVG -I/opt/netsurf/include")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DWITH_SVG -I/opt/netsurf/include")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/netsurf/lib")
	set(COMMON_SOURCES ${COMMON_SOURCES} src/svg.cpp)
endif()

add_library(mo2-engine STATIC ${COMMON_SOURCES})

if(DEMO)
	set(EXENAME MonsterRPG2Demo${M2_SUFFIX})
else()
	set(EXENAME MonsterRPG2${M2_SUFFIX})
endif()

set(EXE_SOURCES src/monster2.cpp)
if(WIN32)
	list(APPEND EXE_SOURCES icons/windows_icon.rc)
endif()

add_executable(${EXENAME} ${EXE_SOURCES})

if(KCM_AUDIO)
	if(NOT WIN32)
		LIST(APPEND EXTRA_LIBS "asound" "pulse-simple" "pulse" "vorbisfile" "vorbis" "ogg")
	endif()
endif()

if(WIN32)
	set(CFLAGS "/MT /MP /D_USE_MATH_DEFINES /DALLEGRO_STATICLINK /EHsc /D_WIN32_WINNT=0x501")

	if(KCM_AUDIO)
		set(CFLAGS "/DKCM_AUDIO")
	endif()

	if(DEMO)
		set(CFLAGS "${CFLAGS} /DDEMO")
	endif()

	if(STEAMWORKS)
		set(CFLAGS "${CFLAGS} /DSTEAMWORKS")
	endif()

	set_target_properties(mo2-engine PROPERTIES COMPILE_FLAGS "${CFLAGS}")
	set_target_properties(${EXENAME} PROPERTIES COMPILE_FLAGS "${CFLAGS}")

	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO /SUBSYSTEM:WINDOWS")

	list(APPEND EXTRA_LIBS
		"dxguid"
		"dinput8"
		"dsound"
		"d3d9"
		"d3dx9"
		"opengl32"
		"comdlg32"
		"gdi32"
		"gdiplus"
		"kernel32"
		"ole32"
		"psapi"
		"shlwapi"
		"user32"
		"uuid"
		"winmm"
		"lua5.2"
		"freetype"
		"physfs"
		"zlibstatic"
		"allegro_monolith-static"
	)

	if(STEAMWORKS)
		list(APPEND EXTRA_LIBS "steam_api")
	endif()

	SET(MAIN_LIBS 
		${ICON_OBJ}
		mo2-engine
		${EXTRA_LIBS}
	)

	if(KCM_AUDIO)
		LIST(APPEND MAIN_LIBS "ws2_32" "dsound")
	else()
		LIST(APPEND MAIN_LIBS "bass")
	endif(KCM_AUDIO)
else(WIN32)
	if(KCM_AUDIO)
		set(CMAKE_C_FLAGS_RELEASE "-DKCM_AUDIO")
		set(CMAKE_CXX_FLAGS_RELEASE "-DKCM_AUDIO")
	endif()

	if(DEMO)
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DDEMO")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DDEMO")
	endif()

	if(APPLE)
		set(ALLEGRO_MAIN_LIB "allegro_main${ALLEGRO_SUFFIX}")
		FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
		FIND_LIBRARY(FOUNDATION_LIBRARY Foundation)
		FIND_LIBRARY(APPSERVICES_LIBRARY ApplicationServices)
		FIND_LIBRARY(APPKIT_LIBRARY AppKit)
		FIND_LIBRARY(IOKIT_LIBRARY IOKit)
		FIND_LIBRARY(AUDIOTOOLBOX_LIBRARY AudioToolbox)
		FIND_LIBRARY(SECURITY_LIBRARY Security)
		LIST(APPEND EXTRA_LIBS ${IOKIT_LIBRARY} ${OPENGL_LIBRARY} ${FOUNDATION_LIBRARY} ${APPSERVICES_LIBRARY} ${APPKIT_LIBRARY} ${AUDIOTOOLBOX_LIBRARY} ${GAMEKIT_LIBRARY} ${SECURITY_LIBRARY})
		SET(MAIN_LIBS 
			mo2-engine
			allegro_ttf${ALLEGRO_SUFFIX}
			allegro_font${ALLEGRO_SUFFIX}
			allegro_primitives${ALLEGRO_SUFFIX}
			allegro_dialog${ALLEGRO_SUFFIX}
			allegro_memfile${ALLEGRO_SUFFIX}
			allegro_image${ALLEGRO_SUFFIX}
			allegro_physfs${ALLEGRO_SUFFIX}
			allegro${ALLEGRO_SUFFIX} ${ALLEGRO_MAIN_LIB}
			${EXTRA_LIBS}
			/Users/trent/code/local/lib/libfreetype.a
			/Users/trent/code/local/lib/libphysfs.a
			/Users/trent/code/local/lib/libz.a
			/Users/trent/code/local/lib/liblua.a
		)
		if(KCM_AUDIO)
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework OpenAL")
		else()
			LIST(APPEND MAIN_LIBS "bass")
		endif()
	else()
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,\$ORIGIN")
		SET(MAIN_LIBS 
			mo2-engine
			allegro_monolith${ALLEGRO_SUFFIX}
			${EXTRA_LIBS}
			Xcursor Xi Xinerama Xrandr SM ICE X11 Xext
		)
		if(NOT KCM_AUDIO)
			LIST(APPEND MAIN_LIBS "${CMAKE_BINARY_DIR}/libbass.so")
		endif()
		if(STEAMWORKS)
			LIST(APPEND MAIN_LIBS "${CMAKE_BINARY_DIR}/libsteam_api.so")
		endif()
		if(RASPBERRYPI)
			LIST(APPEND MAIN_LIBS GLESv2 EGL bcm_host m pthread freetype physfs png z lua5.2)
		else()
			LIST(APPEND MAIN_LIBS GL GLU physfs freetype z png lua5.2 pthread)
		endif()
	endif()
endif()

target_link_libraries(
	${EXENAME}
	${MAIN_LIBS}
) 

