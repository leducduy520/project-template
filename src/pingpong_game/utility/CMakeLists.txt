cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH THIS_PARENT_PATH)
get_property(PARENT_PROJECT_NAME DIRECTORY ${THIS_PARENT_PATH} PROPERTY PROJECT_NAME)

project(pingpong_game_utility)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")
file(GLOB HEADER_FILES "${THIS_PARENT_PATH}/include/${PROJECT_NAME}/*.hpp" "${THIS_PARENT_PATH}/include/${PROJECT_NAME}/*.h")

file(GLOB RESOURCE_FILES LIST_DIRECTORIES false
    "${THIS_PARENT_PATH}/resources/*.*"
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(
    ${PROJECT_NAME} PUBLIC sfml-graphics sfml-audio rapidcsv magic_enum nlohmann_json::nlohmann_json mongo::bsoncxx_static mongo::bson_static
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PARENT_PROJECT_NAME}/${PROJECT_NAME}>
    PUBLIC $<BUILD_INTERFACE:${THIS_PARENT_PATH}/include/${PROJECT_NAME}>
)

target_set_warnings(
    TARGET ${PROJECT_NAME} ENABLE ${ENABLE_WARNINGS} AS_ERRORS
    ${ENABLE_WARNINGS_AS_ERRORS}
)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PUBLIC -fPIC)
endif(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")

target_sources(
    ${PROJECT_NAME} PRIVATE FILE_SET ${PROJECT_NAME}_headers TYPE HEADERS
    BASE_DIRS ${THIS_PARENT_PATH}/include # must be set
    FILES ${HEADER_FILES}
)

target_sources(${PROJECT_NAME} PRIVATE ${RESOURCE_FILES})
source_group("Resource Files" FILES ${RESOURCE_FILES})

# source_group(" Header Files " FILES ${HEADER_FILES})
if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT " Copy OpenAL DLL "
        PRE_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy
        ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM
    )
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMENT " Copy resources "
        COMMAND
        ${CMAKE_COMMAND} -E copy_directory_if_different
        ${THIS_PARENT_PATH}/resources
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
        VERBATIM
    )
endif(WIN32)

if(ENABLE_CLANG_TIDY)
    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES CXX_CLANG_TIDY
        " clang-tidy;--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy "
    )
endif(ENABLE_CLANG_TIDY)

install(
    TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
)

install(DIRECTORY ${THIS_PARENT_PATH}/resources/ DESTINATION " resources " COMPONENT Resources)
