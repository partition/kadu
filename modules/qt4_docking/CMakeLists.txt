set (SOURCES
	qt4_docking.cpp
)

set (MOC_SOURCES
	qt4_docking.h
)

kadu_module (qt4_docking
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES docking
	MODULE_BUILDDEF QT4DOCKAPI_BUILD
)
