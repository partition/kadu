set (storage_SRCS
	named-storable-object.cpp
	module-data.cpp
	shared.cpp
	shared-base.cpp
	storable-object.cpp
	storable-string-list.cpp
	storage-point.cpp
	uuid-storable-object.cpp
)

set (storage_MOC_SRCS
	module-data.h
)

qt_wrap_cpp (storage storage_MOC_FILES ${storage_MOC_SRCS})

add_library (storage
	STATIC
	${storage_SRCS}
	${storage_MOC_FILES}
)

set_target_properties (storage PROPERTIES ENABLE_EXPORTS ON)
set_target_properties (storage PROPERTIES COMPILE_FLAGS "-DKADULIB")
