set (avatars_SRCS
	avatar.cpp
	avatar-job-manager.cpp
	avatar-job-runner.cpp
	avatar-manager.cpp
	avatar-shared.cpp
)

set (avatars_MOC_SRCS
	avatar-job-manager.h
	avatar-job-runner.h
	avatar-manager.h
	avatar-shared.h
)

qt_wrap_cpp (avatars avatars_MOC_FILES ${avatars_MOC_SRCS})

add_library (avatars
	STATIC
	${avatars_SRCS}
	${avatars_MOC_FILES}
)

set_target_properties (avatars PROPERTIES ENABLE_EXPORTS ON)
set_target_properties (avatars PROPERTIES COMPILE_FLAGS "-DKADULIB")
