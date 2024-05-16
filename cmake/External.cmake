function(read_deps_json)
    file(READ ${CMAKE_SOURCE_DIR}/deps.json DEPS_JSON_STRING)

    string(JSON DEPS_LENGTH LENGTH ${DEPS_JSON_STRING})
    math(EXPR LAST_INDX "${DEPS_LENGTH} - 1" OUTPUT_FORMAT DECIMAL)

    if(${LAST_INDX} STREQUAL "-1")
        message("deps.json is empty or parsed unsuccessfully")
        set(PKG_LAST_INDEX
            NO_FOUND
            PARENT_SCOPE
            )
        return()
    endif()

    set(PKG_LAST_INDEX
        ${LAST_INDX}
        PARENT_SCOPE
        )

    foreach(ITR RANGE ${LAST_INDX})
        string(JSON LABEL GET ${DEPS_JSON_STRING} ${ITR} label)
        list(APPEND LABELS ${LABEL})
        string(JSON REPO GET ${DEPS_JSON_STRING} ${ITR} repo)
        list(APPEND REPOS ${REPO})
        string(JSON TAG GET ${DEPS_JSON_STRING} ${ITR} tag)
        list(APPEND TAGS ${TAG})
        string(
            JSON
            SHALLOW
            ERROR_VARIABLE
            ERROR
            GET
            ${DEPS_JSON_STRING}
            ${ITR}
            shallow
            )
        if(ERROR)
            set(SHALLOW FALSE)
        endif()
        list(APPEND SHALLOWS ${SHALLOW})
    endforeach()

    set(PKG_TAGS
        ${TAGS}
        PARENT_SCOPE
        )
    set(PKG_LABELS
        ${LABELS}
        PARENT_SCOPE
        )
    set(PKG_REPOS
        ${REPOS}
        PARENT_SCOPE
        )
    set(PKG_SHALLOWS
        ${SHALLOWS}
        PARENT_SCOPE
        )
endfunction(read_deps_json)

macro(build_external_project)
    set(multiValueArgs REPOS TAGS LABELS SHALLOWS)
    set(oneValueArgs FIRST_INDX LAST_INDX)
    set(options READ_FROM_JSON SUB_PROJECT)
    cmake_parse_arguments(EXTERNAL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(EXTERNAL_READ_FROM_JSON)
        read_deps_json()
        set(EXTERNAL_REPOS ${PKG_REPOS})
        set(EXTERNAL_TAGS ${PKG_TAGS})
        set(EXTERNAL_LABELS ${PKG_LABELS})
        set(EXTERNAL_SHALLOWS ${PKG_SHALLOWS})
        set(EXTERNAL_FIRST_INDX 0)
        set(EXTERNAL_LAST_INDX ${PKG_LAST_INDEX})
    endif(EXTERNAL_READ_FROM_JSON)

    if(EXTERNAL_SUB_PROJECT)
        set(EXTERNAL_ADD_FILE ${CMAKE_SOURCE_DIR}/cmake/ExternalAddSub.txt.in)
        #set(MODULE_STRING "include(FetchContent)\nset(FETCHCONTENT_BASE_DIR \"${CMAKE_CURRENT_BINARY_DIR}\" CACHE INTERNAL \"\")")
        set(MODULE_STRING "include(FetchContent)")
    else(EXTERNAL_SUB_PROJECT)
        set(EXTERNAL_ADD_FILE ${CMAKE_SOURCE_DIR}/cmake/ExternalAdd.txt.in)
        set(MODULE_STRING "include(ExternalProject)\nset_directory_properties(PROPERTIES EP_BASE )")
    endif(EXTERNAL_SUB_PROJECT)

    if(${EXTERNAL_LAST_INDX} MATCHES "(-1|NO_FOUND)")
        message("Packages list's last index is not found")
        return()
    endif()

    if(${EXTERNAL_LAST_INDX} MATCHES "(-1|NO_FOUND)")
        set(EXTERNAL_FIRST_INDX 0)
    endif()

    set(INIT_STRING "cmake_minimum_required(VERSION 3.27)\nproject(external)\n${MODULE_STRING}\n")
    file(WRITE ${EXTERNAL_DIR}/CMakeLists.txt ${INIT_STRING})
    unset(INIT_STRING)

    foreach(ITR RANGE ${EXTERNAL_FIRST_INDX} ${EXTERNAL_LAST_INDX})
        list(GET EXTERNAL_REPOS ${ITR} EXTERNAL_REPO)
        list(GET EXTERNAL_TAGS ${ITR} EXTERNAL_BRANCH)
        list(GET EXTERNAL_LABELS ${ITR} EXTERNAL_LABEL)
        list(GET EXTERNAL_SHALLOWS ${ITR} EXTERNAL_SHALLOW)

        configure_file(${EXTERNAL_ADD_FILE} ${EXTERNAL_DIR}/ExternalAdd-${EXTERNAL_LABELS}.txt @ONLY)
        file(READ ${EXTERNAL_DIR}/ExternalAdd-${EXTERNAL_LABELS}.txt EXTERNAL_ADD_STRING)
        file(REMOVE ${EXTERNAL_DIR}/ExternalAdd-${EXTERNAL_LABELS}.txt)
        file(APPEND ${EXTERNAL_DIR}/CMakeLists.txt "${EXTERNAL_ADD_STRING}\n")
    endforeach()

    if(NOT EXTERNAL_SUB_PROJECT)
        configure_file(${CMAKE_SOURCE_DIR}/cmake/ExternalCMakePresets.json.in ${EXTERNAL_DIR}/CMakePresets.json)

        execute_process(
            COMMAND ${CMAKE_COMMAND} --no-warn-unused-cli --preset=${PRESET} WORKING_DIRECTORY ${EXTERNAL_DIR}
            )

        execute_process(
            COMMAND ${CMAKE_COMMAND} --build --preset=${PRESET}
            WORKING_DIRECTORY ${EXTERNAL_DIR}
            RESULT_VARIABLE RESULT
            ERROR_VARIABLE ERROR
            )

        if(NOT (${RESULT} EQUAL 0))
            message(FATAL_ERROR "Error message: ${ERROR}")
        endif()

    else(NOT EXTERNAL_SUB_PROJECT)
        add_subdirectory(${EXTERNAL_DIR})

    endif(NOT EXTERNAL_SUB_PROJECT)

endmacro()
