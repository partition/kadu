set (emoticons_SRCS
	emoticon-selector-button.cpp
	emoticon-selector.cpp
	emoticons-manager.cpp
	emots-walker.cpp
	prefix-node.cpp
)

set (emoticons_MOC_SRCS
	emoticon-selector-button.h
	emoticon-selector.h
	emoticons-manager.h
)

qt_wrap_cpp (emoticons emoticons_MOC_FILES ${emoticons_MOC_SRCS})

add_library (emoticons
	STATIC
	${emoticons_SRCS}
	${emoticons_MOC_FILES}
)

set_target_properties (emoticons PROPERTIES ENABLE_EXPORTS ON)
set_target_properties (emoticons PROPERTIES COMPILE_FLAGS "-DKADULIB")
