set (kadu_SRCS
	activate.cpp
	debug.cpp
	dnshandler.cpp
	html_document.cpp
	icons-manager.cpp
	kadu-application.cpp
	languages-manager.cpp
	modules.cpp
	themes.cpp
	updates.cpp
)

if (UNIX AND NOT APPLE)
	set (kadu_SRCS
		${kadu_SRCS}
		os/x11tools.cpp
	)
endif (UNIX AND NOT APPLE)
if (NOT WIN32)
	set (kadu_SRCS
		${kadu_SRCS}
		os/qtsingleapplication/qtlocalpeer.cpp
		os/qtsingleapplication/qtlockedfile.cpp
	)
endif (NOT WIN32)

set (kadu_MOC_SRCS
	dnshandler.h
	icons-manager.h
	kadu-application.h
	modules.h
	themes.h
	updates.h
)

set (kadu_SUBDIRS
	accounts
	avatars
	buddies
	chat
	configuration
	contacts
	core
	emoticons
	file-transfer
	gui
	identities
	misc
	model
	multilogon
	notify
	os
	parser
	protocols
	qt
	status
	storage
	themes
	url-handlers
)

if (UNIX AND NOT APPLE)
	include (FindX11)
	if (X11_FOUND)
		message (STATUS "Found X11 libraries: ${X11_X11_LIB}")
	else (X11_Xfixes_FOUND)
		message (FATAL_ERROR "Could not find X11 libraries")
	endif (X11_FOUND)

	if (USE_X11_XFIXES)
		if (X11_Xfixes_FOUND)
			message (STATUS "Found X11 Xfixes extension: ${X11_Xfixes_LIB}")
		else (X11_Xfixes_FOUND)
			message (FATAL_ERROR "Could not find X11 Xfixes extension")
		endif (X11_Xfixes_FOUND)
	endif (USE_X11_XFIXES)

	if (NOT DESKTOP_ENTRY)
		set (DESKTOP_ENTRY ${CMAKE_CURRENT_BINARY_DIR}/kadu.desktop)
	endif (NOT DESKTOP_ENTRY)
	add_custom_command (OUTPUT ${DESKTOP_ENTRY}
		COMMAND touch ${DESKTOP_ENTRY}
		COMMAND sh ${CMAKE_SOURCE_DIR}/varia/scripts/create_kadu_desktop.sh ${CMAKE_INSTALL_PREFIX} > ${DESKTOP_ENTRY}
		DEPENDS ${CMAKE_SOURCE_DIR}/varia/scripts/create_kadu_desktop.sh
		COMMENT "Generating desktop entry file"
	)
	add_custom_target (DESKTOP_ENTRY_FILE ALL
		DEPENDS ${DESKTOP_ENTRY}
	)
endif (UNIX AND NOT APPLE)

qt_wrap_cpp (kadu MOC_FILES ${kadu_MOC_SRCS})

if (WIN32)
	set (kadu_SRCS
		${kadu_SRCS}
		os/qtsingleapplication/qtlocalpeer.cpp
		os/qtsingleapplication/qtlockedfile.cpp
		os/qtsingleapplication/qtlockedfile_win.cpp
	)

	add_library (kadu_core SHARED ${kadu_SRCS} ${MOC_FILES} kadu_core.rc)
	kadu_source_subdirectories (kadu_core ${kadu_SUBDIRS})

	set_target_properties (kadu_core PROPERTIES DEFINE_SYMBOL KADULIB)
	set_target_properties (kadu_core PROPERTIES COMPILE_FLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}/includes")
	target_link_libraries (kadu_core ${STATIC_MODULES} ${QT_LIBRARIES})
	set_property (TARGET kadu_core PROPERTY LINK_INTERFACE_LIBRARIES "")

	if (MINGW)
		exec_program (windres ${CMAKE_CURRENT_SOURCE_DIR}
			ARGS "kadu_mingw.rc kadu_rc.o"
			OUTPUT_VARIABLE NULL_VAR
		)
		add_executable (kadu WIN32 main.cpp main_win32.cpp kadu_rc.o)
		add_executable (kadu_c main.cpp main_win32.cpp)
	else (MINGW)
		add_executable (kadu WIN32 main.cpp main_win32.cpp kadu.rc)
		add_executable (kadu_c main.cpp main_win32.cpp kadu.rc)
	endif (MINGW)

	set_target_properties (kadu_c PROPERTIES COMPILE_FLAGS "-DKADU_CONSOLE")

	target_link_libraries (kadu kadu_core ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} ws2_32)
	target_link_libraries (kadu_c kadu_core ${QT_LIBRARIES} ws2_32)

	install (TARGETS kadu_core
		RUNTIME DESTINATION ${KADU_BIN_DIR}
		ARCHIVE DESTINATION ${KADU_DATA_DIR}/kadu-sdk/lib
		LIBRARY DESTINATION ${KADU_DATA_DIR}/kadu-sdk/lib
	)
	install (TARGETS kadu_c DESTINATION ${KADU_BIN_DIR})
	install (DIRECTORY "."
		DESTINATION ${KADU_DATA_DIR}/kadu-sdk/include/kadu-core
		FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE
	)
else (WIN32)
	add_executable (kadu main.cpp main_unix.cpp os/qtsingleapplication/qtlockedfile_unix.cpp ${kadu_SRCS} ${MOC_FILES})

	kadu_source_subdirectories (kadu ${kadu_SUBDIRS})

	set_target_properties (kadu PROPERTIES ENABLE_EXPORTS ON)
	set_target_properties (kadu PROPERTIES COMPILE_FLAGS "-DKADULIB")

	target_link_libraries (kadu ${STATIC_MODULES} ${QT_LIBRARIES} ${CMAKE_DL_LIBS})

	if (UNIX AND NOT APPLE)
		target_link_libraries (kadu ${X11_X11_LIB})

		if (USE_X11_XFIXES)
			target_link_libraries (kadu ${X11_Xfixes_LIB})
		endif (USE_X11_XFIXES)
	endif (UNIX AND NOT APPLE)
endif (WIN32)


install (TARGETS kadu DESTINATION ${KADU_BIN_DIR})

# install desktop file
if (UNIX AND NOT APPLE)
	install (FILES ${DESKTOP_ENTRY} DESTINATION share/applications)

	# install icons
	install (FILES hi16-app-kadu.png DESTINATION "share/icons/hicolor/16x16/apps" RENAME kadu.png)
	install (FILES hi22-app-kadu.png DESTINATION "share/icons/hicolor/22x22/apps" RENAME kadu.png)
	install (FILES hi24-app-kadu.png DESTINATION "share/icons/hicolor/24x24/apps" RENAME kadu.png)
	install (FILES hi32-app-kadu.png DESTINATION "share/icons/hicolor/32x32/apps" RENAME kadu.png)
	install (FILES hi48-app-kadu.png DESTINATION "share/icons/hicolor/48x48/apps" RENAME kadu.png)
	install (FILES hi64-app-kadu.png DESTINATION "share/icons/hicolor/64x64/apps" RENAME kadu.png)
	install (FILES hi128-app-kadu.png DESTINATION "share/icons/hicolor/128x128/apps" RENAME kadu.png)
	# As of icon-theme-0.5 maximum supported pixmap size is 192, for anything larger one should use SVG.
	#install (FILES hi256-app-kadu.png DESTINATION "share/icons/hicolor/256x256/apps" RENAME kadu.png)
endif (UNIX AND NOT APPLE)

install (FILES "chat/style-engines/chat-scripts.js" DESTINATION ${KADU_DATA_DIR}/scripts)

if (APPLE)
	pkg_search_module (SNDFILE REQUIRED sndfile>=1.0)
	include_directories (${SNDFILE_INCLUDE_DIRS})
	link_directories (${SNDFILE_LIBRARY_DIRS})
	target_link_libraries (kadu "-framework carbon")
endif (APPLE)
