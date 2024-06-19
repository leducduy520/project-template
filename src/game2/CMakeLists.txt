add_library(GameManager SHARED GameManager.cpp entity.cpp ball.cpp background.cpp paddle.cpp brick.cpp)

target_include_directories(GameManager PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(GameManager sfml-graphics sfml-window sfml-system)

set_target_properties(GameManager PROPERTIES 
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    PUBLIC_HEADER "include/GameManager.hpp")


if(WIN32)
    add_custom_command(
        TARGET GameManager
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:GameManager>
        VERBATIM)
endif()

add_custom_command(
    TARGET GameManager POST_BUILD
    COMMENT "Copy resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
    VERBATIM
)

install(
    TARGETS GameManager
    EXPORT GameManager-targets
    RUNTIME
        DESTINATION  ${CMAKE_INSTALL_BINDIR}
        COMPONENT Runtime
    ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Development
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Development
    PUBLIC_HEADER
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/game2
        COMPONENT PublicHeader
)

install (DIRECTORY resources DESTINATION ${CMAKE_INSTALL_BINDIR}/resources)