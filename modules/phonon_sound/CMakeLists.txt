set (SOURCES
	phonon-module.cpp
	phonon-player.cpp
)

set (MOC_SOURCES
	phonon-player.h
)

set (QT_USE_PHONON 1)

if (NOT QT_PHONON_FOUND)
	pkg_search_module (PHONON REQUIRED phonon)
endif (NOT QT_PHONON_FOUND)

kadu_module (phonon_sound
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_LIBRARIES ${QT_PHONON_LIBRARY}
	MODULE_DEPENDENCIES sound
)
