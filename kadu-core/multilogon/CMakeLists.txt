set (multilogon_SRCS
	model/multilogon-model.cpp

	multilogon-session.cpp
)

set (multilogon_MOC_SRCS
	model/multilogon-model.h

	multilogon-session.h
)

qt_wrap_cpp (multilogon multilogon_MOC_FILES ${multilogon_MOC_SRCS})

add_library (multilogon
	STATIC
	${multilogon_SRCS}
	${multilogon_MOC_FILES}
)

set_target_properties (multilogon PROPERTIES ENABLE_EXPORTS ON)
set_target_properties (multilogon PROPERTIES COMPILE_FLAGS "-DKADULIB")
