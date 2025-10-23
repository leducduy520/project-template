macro(external_pre_set)
    include(ExternalProject)

    if(NOT ARG_NAME
       OR NOT ARG_GIT_REPOSITORY
       OR NOT ARG_GIT_TAG
       OR NOT ARG_PREFIX
       )
        message(
            FATAL_ERROR
                "add_header_only_library: NAME, GIT_REPOSITORY, GIT_TAG and PREFIX are required"
            )
    endif()

    set(INSTALL_DIR "${ARG_PREFIX}-install")
    set(BUILD_DIR "${ARG_PREFIX}-build")
endmacro(external_pre_set)

macro(external_post_set)
    # ------------------------------------------------------------------
    # Export the three directories to the caller
    # ------------------------------------------------------------------
    set(${ARG_NAME}_BIN_DIR "${INSTALL_DIR}/bin" PARENT_SCOPE)
    set(${ARG_NAME}_LIB_DIR "${INSTALL_DIR}/lib" PARENT_SCOPE)
    set(${ARG_NAME}_INCLUDE_DIR "${INSTALL_DIR}/include" PARENT_SCOPE)

    # ------------------------------------------------------------------
    # (Optional) expose the ExternalProject target name for add_dependencies
    # ------------------------------------------------------------------
    set(${ARG_NAME}_EXTERNAL_TARGET "${ARG_NAME}_external" PARENT_SCOPE)

    set(export_name "${ARG_NAME}_INTERFACE")
    add_library(${export_name} INTERFACE)
    add_dependencies(${export_name} ${ARG_NAME}_external)
    target_link_directories(${export_name} INTERFACE ${INSTALL_DIR}/lib)
    target_include_directories(${export_name} INTERFACE ${INSTALL_DIR}/include)
endmacro(external_post_set)

function(add_header_only_library)

    cmake_parse_arguments(
        ARG # prefix
        "" # options
        "NAME;GIT_REPOSITORY;GIT_TAG;PREFIX" # one-value keywords
        "CMAKE_ARGS" # multi-value keywords
        ${ARGN}
        )

    # ------------------------------------------------------------------
    # ExternalProject preset
    # ------------------------------------------------------------------
    external_pre_set()

    # ------------------------------------------------------------------
    # ExternalProject definition
    # ------------------------------------------------------------------
    ExternalProject_Add(
        "${ARG_NAME}_external"
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        GIT_SHALLOW TRUE
        PREFIX ${BUILD_DIR}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} ${ARG_CMAKE_ARGS}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
            -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config ${CMAKE_BUILD_TYPE}
        INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config ${CMAKE_BUILD_TYPE}
        )

    # ------------------------------------------------------------------
    # ExternalProject postset
    # ------------------------------------------------------------------
    external_post_set()
endfunction()

function(add_shared_library)
    cmake_parse_arguments(
        ARG # prefix
        "" # options
        "NAME;GIT_REPOSITORY;GIT_TAG;PREFIX" # one-value keywords
        "CMAKE_ARGS" # multi-value keywords
        ${ARGN}
        )

    # ------------------------------------------------------------------
    # ExternalProject preset
    # ------------------------------------------------------------------
    external_pre_set()

    # ------------------------------------------------------------------
    # ExternalProject definition
    # ------------------------------------------------------------------
    ExternalProject_Add(
        "${ARG_NAME}_external"
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        GIT_SHALLOW TRUE
        PREFIX ${BUILD_DIR}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DBUILD_SHARED_LIBS=ON
            ${ARG_CMAKE_ARGS}
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_GENERATOR:STRING=${CMAKE_GENERATOR}
            -DCMAKE_GENERATOR_PLATFORM:STRING=${CMAKE_GENERATOR_PLATFORM}
            -DCMAKE_INSTALL_PARALLEL_LEVEL:STRING=${CMAKE_INSTALL_PARALLEL_LEVEL}
            -DCMAKE_BUILD_JOBS:STRING=${CMAKE_BUILD_JOBS}
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config ${CMAKE_BUILD_TYPE}
        INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config ${CMAKE_BUILD_TYPE}
        )

    # ------------------------------------------------------------------
    # ExternalProject postset
    # ------------------------------------------------------------------
    external_post_set()
endfunction()

macro(FetchLib)
    add_header_only_library(
        NAME
        rapidcsv
        GIT_REPOSITORY
        https://github.com/d99kris/rapidcsv.git
        GIT_TAG
        v8.82
        PREFIX
        ${EXTERNAL_PROJECT_DIR}/rapidcsv
        )
    add_header_only_library(
        NAME
        magic_enum
        GIT_REPOSITORY
        https://github.com/Neargye/magic_enum.git
        GIT_TAG
        v0.9.6
        PREFIX
        ${EXTERNAL_PROJECT_DIR}/magic_enum
        )
    add_header_only_library(
        NAME
        nlohmann_json
        GIT_REPOSITORY
        https://github.com/nlohmann/json.git
        GIT_TAG
        v3.11.3
        PREFIX
        ${EXTERNAL_PROJECT_DIR}/nlohmann_json
        )

    add_shared_library(
        NAME
        SFML
        GIT_REPOSITORY
        "https://github.com/SFML/SFML.git"
        GIT_TAG
        "2.6.x"
        PREFIX
        "${EXTERNAL_PROJECT_DIR}/SFML"
        CMAKE_ARGS
        -DSFML_BUILD_SYSTEM=ON
        -DSFML_BUILD_WINDOW=ON
        -DSFML_BUILD_GRAPHICS=ON
        -DSFML_BUILD_AUDIO=ON
        -DSFML_BUILD_NETWORK=OFF
        )
    target_link_libraries(SFML_INTERFACE INTERFACE sfml-graphics sfml-audio sfml-window sfml-system)
endmacro(FetchLib)
