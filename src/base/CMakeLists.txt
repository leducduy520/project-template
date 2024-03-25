project(base_src)

file(GLOB SRC_FILES CONFIGURE_DEPENDS "*.cpp" "*.c")

add_library(base SHARED ${SRC_FILES})
