set (os_SRCS
	generic/compositing-aware-object.cpp
	generic/system-info.cpp
	generic/url-opener.cpp
)

set (os_MOC_SRCS
	qtsingleapplication/qtlocalpeer.h
)

qt_wrap_cpp (os os_MOC_FILES ${os_MOC_SRCS})

add_library (os
	STATIC
	${os_SRCS}
	${os_MOC_FILES}
)

set_target_properties (os PROPERTIES ENABLE_EXPORTS ON)
set_target_properties (os PROPERTIES COMPILE_FLAGS "-DKADULIB")
