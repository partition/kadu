set (core_SRCS
	core.cpp
	core-actions.cpp
	crash-aware-object.cpp
)

set (core_MOC_SRCS
	core.h
)

qt_wrap_cpp (core core_MOC_FILES ${core_MOC_SRCS})

add_library (core
	STATIC
	${core_SRCS}
	${core_MOC_FILES}
)

set_target_properties (core PROPERTIES ENABLE_EXPORTS ON)
set_target_properties (core PROPERTIES COMPILE_FLAGS "-DKADULIB")
