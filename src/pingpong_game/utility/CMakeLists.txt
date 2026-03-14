# Project setup
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH THIS_PARENT_PATH)
get_property(PARENT_PROJECT_NAME DIRECTORY ${THIS_PARENT_PATH} PROPERTY PROJECT_NAME)

project(pingponggameutility)

# Source files
set(SOURCE_FILES
    src/background.cpp
    src/ball.cpp
    src/brick.cpp
    src/countingtext.cpp
    src/entity.cpp
    src/entitymanager.cpp
    src/helper.cpp
    src/interactions.cpp
    src/paddle.cpp
    src/soundplayer.cpp
    src/threadpool.cpp
)

set(HEADER_FILES
    include/background.hpp
    include/ball.hpp
    include/brick.hpp
    include/constants.hpp
    include/countingtext.hpp
    include/entity.hpp
    include/entitymanager.hpp
    include/helper.hpp
    include/interactions.hpp
    include/paddle.hpp
    include/resource_integrity.hpp
    include/soundplayer.hpp
    include/threadpool.hpp
    include/timer.hpp
)

# Library target
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

# Dependencies
find_path(RAPIDCSV_INCLUDE_DIRS "rapidcsv.h")
find_path(PICOSHA2_INCLUDE_DIRS "picosha2.h")
find_package(nlohmann_json CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(SFML COMPONENTS Graphics Audio CONFIG REQUIRED)
find_package(boost_thread CONFIG REQUIRED)
find_package(boost_asio CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        nlohmann_json::nlohmann_json
        magic_enum::magic_enum
        SFML::Audio
        SFML::Graphics
        Boost::thread
        Boost::asio
        fmt::fmt
        spdlog::spdlog_header_only
)

target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_NO_EXCEPTIONS)

# Includes
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PARENT_PROJECT_NAME}/${PROJECT_NAME}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    INTERFACE
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/interface>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/interface>
        ${RAPIDCSV_INCLUDE_DIRS}
        ${PICOSHA2_INCLUDE_DIRS}
)

# Compiler options
target_set_warnings(
    TARGET ${PROJECT_NAME}
    ENABLE ${ENABLE_WARNINGS}
    AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS}
)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PUBLIC -fPIC)
endif()

# Properties
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        FOLDER "PingPong Game"
        PUBLIC_HEADER "${HEADER_FILES}"
)

# Clang-tidy
if(ENABLE_CLANG_TIDY AND (THIS_COMPILER_GCC OR THIS_COMPILER_CLANG OR THIS_COMPILER_CLANG_CL))
    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
            CXX_CLANG_TIDY "clang-tidy;--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy"
    )
endif()

# Source organization
target_sources(
    ${PROJECT_NAME}
    PRIVATE
        FILE_SET ${PROJECT_NAME}_headers
        TYPE HEADERS
        BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include
        FILES ${HEADER_FILES}
)

# Install
install(
    TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PARENT_PROJECT_NAME}/${PROJECT_NAME}
                  COMPONENT Development
)
