project(widget)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")
file(GLOB HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/include/*.hpp"
     "${CMAKE_CURRENT_LIST_DIR}/include/*.h"
     )

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC sfml-graphics)

target_include_directories(
    ${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    )

target_set_warnings(
    TARGET ${PROJECT_NAME} ENABLE ${ENABLE_WARNINGS} AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS}
    )

set_target_properties(${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

# set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Widget")
if(ENABLE_CLANG_TIDY AND (THIS_COMPILER_GCC OR THIS_COMPILER_CLANG OR THIS_COMPILER_CLANG_CL))
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES CXX_CLANG_TIDY
                                   "clang-tidy;--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy"
        )
endif()

target_sources(
    ${PROJECT_NAME} PRIVATE FILE_SET ${PROJECT_NAME}_headers TYPE HEADERS FILES ${HEADER_FILES}
    )

install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    )
